using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Runtime.ExceptionServices;
using System.Runtime.InteropServices;

namespace LiteScript.Interop
{
    // Errors and callbacks between the native library and .NET
    internal static unsafe class Native
    {
        // The callbacks given to the native library (kept alive by these fields)
        private static readonly HostInvoke InvokeCallback = Invoke;
        private static readonly HostRelease ReleaseCallback = Release;
        private static readonly HostDebugHook DebugHookCallback = DebugHook;
        private static readonly HostErrorHook ErrorHookCallback = ErrorHook;

        // The states created by the application (to find the State of a native pointer)
        private static readonly ConcurrentDictionary<IntPtr, WeakReference<State>> States = new ConcurrentDictionary<IntPtr, WeakReference<State>>();

        // An exception of a hook given to the native library, thrown again when the native call returns
        [ThreadStatic]
        private static Exception? pendingException;

        // The last exception of a hook thrown again (must cross the native functions unchanged)
        [ThreadStatic]
        private static Exception? propagatingException;

        static Native()
        {
#if NET
            NativeLibraryResolver.Register();
#endif
            NativeMethods.ls_host_init(
                Marshal.GetFunctionPointerForDelegate(InvokeCallback),
                Marshal.GetFunctionPointerForDelegate(ReleaseCallback),
                Marshal.GetFunctionPointerForDelegate(DebugHookCallback),
                Marshal.GetFunctionPointerForDelegate(ErrorHookCallback));
        }

        /// <summary>
        /// Initialize the callbacks of the native library (called before the first use)
        /// </summary>
        public static void EnsureInitialized()
        {
            // The static constructor does the work
        }

        ////// ERRORS //////

        /// <summary>
        /// Throw the exception that corresponds to a status
        /// </summary>
        /// <param name="status">The status returned by a native function</param>
        /// <param name="memory">The memory that receives the value of a script error</param>
        public static void Check(int status, Memory? memory = null)
        {
            if (status == NativeMethods.Ok)
                return;
            throw CreateException(status, memory);
        }

        private static Exception CreateException(int status, Memory? memory)
        {
            string message = Utf8.Decode(NativeMethods.ls_error_message());
            switch (status)
            {
                case NativeMethods.ScriptError:
                {
                    NativeMethods.ls_error_position(out uint line, out uint column);
                    IntPtr handle = NativeMethods.ls_error_value();
                    Variable? value = null;
                    if (memory != null)
                        value = Variable.FromHandle(memory, handle);
                    else if (handle != IntPtr.Zero)
                        NativeMethods.ls_var_delete(handle);
                    NativeMethods.ls_error_clear();
                    pendingException = null;
                    return new ScriptException(message, value, new SourcePosition(line, column));
                }
                case NativeMethods.SerializationError:
                    NativeMethods.ls_error_clear();
                    pendingException = null;
                    return new SerializationException(message);
                case NativeMethods.InvalidArgument:
                    NativeMethods.ls_error_clear();
                    if (message == "index out of range")
                        return new ArgumentOutOfRangeException("index", message);
                    return new ArgumentException(message);
                case NativeMethods.HostError:
                {
                    NativeMethods.ls_error_clear();
                    Exception? exception = pendingException;
                    pendingException = null;
                    if (exception == null)
                        return new InvalidOperationException(message);
                    propagatingException = exception;
                    ExceptionDispatchInfo.Capture(exception).Throw();
                    return exception;
                }
                default:
                    NativeMethods.ls_error_clear();
                    pendingException = null;
                    return new InvalidOperationException(message);
            }
        }

        // Give an exception of the host to the native library
        private static int Report(Exception exception, bool hook)
        {
            if (exception is ScriptException scriptException)
            {
                IntPtr value = scriptException.Value != null && !scriptException.Value.IsDisposed ? scriptException.Value.Handle : IntPtr.Zero;
                NativeMethods.ls_host_set_error(Utf8.Encode(scriptException.Message), value, scriptException.Position.Line, scriptException.Position.Column);
                GC.KeepAlive(scriptException.Value);
                return NativeMethods.ScriptError;
            }
            NativeMethods.ls_host_set_error(Utf8.Encode(exception.Message), IntPtr.Zero, 0, 0);
            if (hook || ReferenceEquals(exception, propagatingException))
            {
                // The exception reaches the caller of the native library unchanged
                pendingException = exception;
                return NativeMethods.HostError;
            }
            // The exceptions of the native functions become errors of the script
            return NativeMethods.RuntimeError;
        }

        ////// STATES //////

        public static void RegisterState(State state)
        {
            States[state.Handle] = new WeakReference<State>(state);
        }

        public static void UnregisterState(IntPtr handle)
        {
            States.TryRemove(handle, out _);
        }

        private static State GetState(IntPtr handle)
        {
            if (States.TryGetValue(handle, out WeakReference<State>? reference) && reference.TryGetTarget(out State? state))
                return state;
            throw new InvalidOperationException("unknown state");
        }

        ////// HOST OBJECTS //////

        /// <summary>
        /// Return a handle that keeps an object alive until the native library releases it
        /// </summary>
        public static IntPtr Alloc(object target)
        {
            return GCHandle.ToIntPtr(GCHandle.Alloc(target));
        }

        private static void Release(IntPtr handle)
        {
            try
            {
                GCHandle.FromIntPtr(handle).Free();
            }
            catch
            {
                // Nothing can be reported to the native library
            }
        }

        private static int Invoke(IntPtr function, IntPtr statePtr, IntPtr* args, UIntPtr count, IntPtr* result)
        {
            *result = IntPtr.Zero;
            var arguments = new List<Variable?>();
            try
            {
                var target = (NativeFunction)GCHandle.FromIntPtr(function).Target!;
                State state = GetState(statePtr);
                int n = (int)count.ToUInt64();
                for (int i = 0; i < n; i++)
                    arguments.Add(Variable.FromHandle(state.Memory, NativeMethods.ls_var_duplicate(args[i])));
                Variable? value = target(state, arguments);
                if (value != null)
                {
                    *result = NativeMethods.ls_var_duplicate(value.Handle);
                    GC.KeepAlive(value);
                }
                return NativeMethods.Ok;
            }
            catch (Exception e)
            {
                return Report(e, false);
            }
            finally
            {
                // The arguments are only valid during the call
                foreach (Variable? argument in arguments)
                    argument?.Dispose();
            }
        }

        private static int DebugHook(IntPtr hook, IntPtr statePtr)
        {
            try
            {
                var action = (Action<State>)GCHandle.FromIntPtr(hook).Target!;
                action(GetState(statePtr));
                return NativeMethods.Ok;
            }
            catch (Exception e)
            {
                return Report(e, true);
            }
        }

        private static int ErrorHook(IntPtr hook, IntPtr statePtr, IntPtr value, IntPtr message, uint line, uint column)
        {
            try
            {
                var action = (Action<State, ScriptException>)GCHandle.FromIntPtr(hook).Target!;
                State state = GetState(statePtr);
                Variable? thrown = Variable.FromHandle(state.Memory, value != IntPtr.Zero ? NativeMethods.ls_var_duplicate(value) : IntPtr.Zero);
                action(state, new ScriptException(Utf8.Decode(message), thrown, new SourcePosition(line, column)));
                return NativeMethods.Ok;
            }
            catch (Exception e)
            {
                return Report(e, true);
            }
        }
    }
}
