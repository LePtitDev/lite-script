#if NET
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace LiteScript.Interop
{
    // Find the native library in runtimes/<rid>/native next to the assembly when the default probing
    // fails (applications that copy the "runtimes" folder without a deps.json entry, plugins...)
    internal static class NativeLibraryResolver
    {
        private static bool registered;

        public static void Register()
        {
            if (registered)
                return;
            registered = true;
            try
            {
                NativeLibrary.SetDllImportResolver(typeof(NativeLibraryResolver).Assembly, Resolve);
            }
            catch (InvalidOperationException)
            {
                // A resolver is already defined by the application
            }
        }

        private static IntPtr Resolve(string name, System.Reflection.Assembly assembly, DllImportSearchPath? searchPath)
        {
            if (name != NativeMethods.Library)
                return IntPtr.Zero;
            if (NativeLibrary.TryLoad(name, assembly, searchPath, out IntPtr handle))
                return handle;
            string? directory = Path.GetDirectoryName(assembly.Location);
            if (string.IsNullOrEmpty(directory))
                return IntPtr.Zero;
            string file;
            string os;
            if (OperatingSystem.IsWindows())
            {
                os = "win";
                file = name + ".dll";
            }
            else if (OperatingSystem.IsMacOS())
            {
                os = "osx";
                file = "lib" + name + ".dylib";
            }
            else
            {
                os = "linux";
                file = "lib" + name + ".so";
            }
            string arch = RuntimeInformation.ProcessArchitecture.ToString().ToLowerInvariant();
            string path = Path.Combine(directory, "runtimes", os + "-" + arch, "native", file);
            if (File.Exists(path) && NativeLibrary.TryLoad(path, out handle))
                return handle;
            return IntPtr.Zero;
        }
    }
}
#endif
