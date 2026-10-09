using System;
using System.Runtime.InteropServices;

namespace LiteScript.Interop
{
    // A number exchanged with the native library
    [StructLayout(LayoutKind.Sequential)]
    internal struct NativeNumber
    {
        public long Integer;
        public double Floating;
        public int IsInteger;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal unsafe delegate int HostInvoke(IntPtr function, IntPtr state, IntPtr* args, UIntPtr count, IntPtr* result);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void HostRelease(IntPtr function);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate int HostDebugHook(IntPtr hook, IntPtr state);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate int HostErrorHook(IntPtr hook, IntPtr state, IntPtr value, IntPtr message, uint line, uint column);

    // The functions of the native library (see dotnet/native/litescript_net.cpp)
    // The texts are null-terminated UTF-8 byte arrays (see Utf8.Encode)
    internal static unsafe class NativeMethods
    {
        public const string Library = "litescript_net";

        // Status of the functions
        public const int Ok = 0;
        public const int ScriptError = 1;
        public const int SerializationError = 2;
        public const int RuntimeError = 3;
        public const int InvalidArgument = 4;
        public const int HostError = 5;

        // The member lists
        public const int MembersArrayNamed = 0;
        public const int MembersClassStatics = 1;
        public const int MembersClassMembers = 2;
        public const int MembersInstanceMembers = 3;
        public const int MembersNamespaceVariables = 4;

        // The lists of variables
        public const int VectorArrayIndexed = 0;
        public const int VectorClassInherits = 1;

        ////// GENERAL //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_free(IntPtr pointer);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_host_init(IntPtr invoke, IntPtr release, IntPtr debugHook, IntPtr errorHook);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_host_set_error(byte[] message, IntPtr value, uint line, uint column);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ls_error_message();

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ls_error_value();

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_error_position(out uint line, out uint column);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_error_clear();

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_release_cached_blocks();

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_type_name(int type, out IntPtr name);

        ////// NUMBERS //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_number_to_string(in NativeNumber n, out IntPtr text);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_number_unary(int op, in NativeNumber a, out NativeNumber result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_number_binary(int op, in NativeNumber a, in NativeNumber b, out NativeNumber result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_number_compare(int op, in NativeNumber a, in NativeNumber b);

        ////// MEMORY //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ls_memory_new(ulong gcThreshold);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_memory_delete(IntPtr memory);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_memory_count(IntPtr memory);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_collect_garbage(IntPtr memory, int ifNeeded, out ulong count);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_memory_set_gc_threshold(IntPtr memory, ulong threshold);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_undefined(IntPtr memory, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_null(IntPtr memory, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_boolean(IntPtr memory, int value, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_number(IntPtr memory, in NativeNumber value, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_string(IntPtr memory, byte[] utf8, UIntPtr length, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_character(IntPtr memory, IntPtr str, ulong index, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_namespace(IntPtr memory, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_class(IntPtr memory, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_instance(IntPtr memory, IntPtr baseClass, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_array(IntPtr memory, IntPtr[] values, IntPtr[]? names, UIntPtr count, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_callback(IntPtr memory, IntPtr function, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_create_callback_named(IntPtr memory, byte[] name, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_register_function(IntPtr memory, byte[] name, IntPtr function, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_has_function(IntPtr memory, byte[] name);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_set_methods(IntPtr memory, int type, IntPtr methods);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_memory_find_method(IntPtr memory, int type, byte[] name, out IntPtr result);

        ////// VARIABLES //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_var_delete(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ls_var_duplicate(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_type(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_same(IntPtr a, IntPtr b);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_var_object_id(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_var_reference_count(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_is_reference(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ls_var_target(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_to_string(IntPtr v, out IntPtr text);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_to_text(IntPtr v, out IntPtr text);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_get_boolean(IntPtr v, out int value);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_get_number(IntPtr v, out NativeNumber value);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_get_string(IntPtr v, out IntPtr text);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_get_string_length(IntPtr v, out ulong length);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_var_set(IntPtr v, int kind, int boolean, in NativeNumber number, byte[]? utf8, UIntPtr length);

        ////// LISTS OF VARIABLES //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_vector_count(IntPtr v, int kind, out ulong count);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_vector_get(IntPtr v, int kind, ulong index, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_vector_set(IntPtr v, int kind, ulong index, IntPtr value);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_vector_insert(IntPtr v, int kind, ulong index, IntPtr value);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_vector_remove(IntPtr v, int kind, ulong index);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_vector_clear(IntPtr v, int kind);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_members_count(IntPtr v, int kind, out ulong count);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_members_name(IntPtr v, int kind, ulong index, out IntPtr name);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_members_value(IntPtr v, int kind, ulong index, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_members_index_of(IntPtr v, int kind, byte[] name, out long index);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_members_find(IntPtr v, int kind, byte[] name, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_members_set(IntPtr v, int kind, byte[] name, IntPtr value);

        ////// CLASSES, INSTANCES, CALLBACKS AND CHARACTERS //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_class_get_operator(IntPtr v, int op, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_class_set_operator(IntPtr v, int op, IntPtr callback);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_class_get_constructor_name(IntPtr v, out IntPtr name);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_class_set_constructor_name(IntPtr v, byte[] name);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_class_define_constructor(IntPtr v, byte[] name, out int defined);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_class_get_constructor(IntPtr v, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_class_inherit(IntPtr v, IntPtr parent);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_instance_get_base(IntPtr v, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_instance_set_base(IntPtr v, IntPtr baseClass);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_callback_is_native(IntPtr v, out int native);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_callback_get_self(IntPtr v, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_callback_set_self(IntPtr v, IntPtr self);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_callback_same_target(IntPtr a, IntPtr b, out int same);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_callback_native_name(IntPtr v, out IntPtr name);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_callback_script_function(IntPtr v, out ulong list, out ulong line, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_character_get(IntPtr v, out IntPtr str, out ulong index);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_character_set(IntPtr v, IntPtr str, ulong index);

        ////// NAMESPACES RESOLUTION //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_namer_find(byte[] buffer, UIntPtr size, byte[] name, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_namer_declare(byte[] buffer, UIntPtr size, byte[] name, IntPtr value);

        ////// OPERATIONS //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_is_truthy(IntPtr v);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_copy(IntPtr memory, IntPtr v, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_clone(IntPtr memory, IntPtr v, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_convert(IntPtr memory, IntPtr v, int type, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_operate(IntPtr memory, int op, IntPtr a, IntPtr b, out IntPtr result,
                                            out IntPtr callback, out IntPtr self, out IntPtr args, out UIntPtr argsSize);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_get_member(IntPtr memory, IntPtr v, byte[] name, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_is_binary(int op);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_values_equal(IntPtr a, IntPtr b, out int equal);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_compare_values(IntPtr a, IntPtr b, out int order);

        ////// STATES //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ls_state_new(IntPtr memory);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_state_delete(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_add_instructions(IntPtr state, byte[] buffer, UIntPtr size, out ulong index);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_add_instruction(IntPtr state, byte[] buffer, UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_state_list_count(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_get_instructions(IntPtr state, ulong index, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_state_list_index(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_state_line(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_execute(IntPtr state, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_execute_single(IntPtr state, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_execute_instruction(IntPtr state, byte[] buffer, UIntPtr size, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_finished(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_call(IntPtr state, IntPtr callee, IntPtr[] args, UIntPtr count, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ls_state_set_max_call_depth(IntPtr state, ulong depth);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_get_variable(IntPtr state, byte[] name, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_declare_variable(IntPtr state, byte[] name, IntPtr value);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ls_state_global_namespace(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_use_namespace(IntPtr state, byte[] path);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_get_this(IntPtr state, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_state_args_count(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_get_arg(IntPtr state, ulong index, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_get_stack(IntPtr state, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ls_state_depth(IntPtr state);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_get_frames(IntPtr state, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_set_debug_hook(IntPtr state, IntPtr hook);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_set_error_hook(IntPtr state, IntPtr hook);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_evaluate(IntPtr state, byte[] buffer, UIntPtr size, ulong maxSteps, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_save(IntPtr state, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_state_load(IntPtr state, byte[] buffer, UIntPtr size);

        ////// SCRIPT //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_script_compile(byte[] code, UIntPtr length, int analyze, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_script_execute(IntPtr state, byte[] code, UIntPtr length, out IntPtr buffer, out UIntPtr size, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_script_error_message(int error, out IntPtr message);

        ////// ASSEMBLY //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_assembly_compile(byte[] code, UIntPtr length, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_assembly_compile_instruction(byte[] line, UIntPtr length, out int error, out IntPtr buffer, out UIntPtr size);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_assembly_disassemble(byte[] buffer, UIntPtr size, out IntPtr code);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_assembly_disassemble_instruction(byte[] buffer, UIntPtr size, out IntPtr code);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_assembly_execute(IntPtr state, byte[] code, UIntPtr length, int after, out IntPtr buffer, out UIntPtr size, out IntPtr result);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_assembly_error_message(int error, out IntPtr message);

        ////// SERIALIZATION //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_save_instructions(byte[] buffer, UIntPtr size, out IntPtr result, out UIntPtr resultSize);

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_load_instructions(byte[] buffer, UIntPtr size, out IntPtr result, out UIntPtr resultSize);

        ////// STANDARD LIBRARY //////

        [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ls_standard_install(IntPtr state, int module);
    }
}
