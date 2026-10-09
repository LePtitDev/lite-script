using System;
using System.Text;

namespace LiteScript.Interop
{
    // Conversions of the texts exchanged with the native library
    internal static unsafe class Utf8
    {
        private static readonly UTF8Encoding Encoding = new UTF8Encoding(false, false);

        /// <summary>
        /// Return a null-terminated UTF-8 text
        /// </summary>
        public static byte[] Encode(string? text)
        {
            text ??= string.Empty;
            byte[] bytes = new byte[Encoding.GetByteCount(text) + 1];
            Encoding.GetBytes(text, 0, text.Length, bytes, 0);
            return bytes;
        }

        /// <summary>
        /// Return the UTF-8 bytes of a text (without terminator) and their number
        /// </summary>
        public static byte[] EncodeData(string? text, out UIntPtr length)
        {
            byte[] bytes = Encoding.GetBytes(text ?? string.Empty);
            length = (UIntPtr)bytes.Length;
            return bytes;
        }

        /// <summary>
        /// Read a null-terminated UTF-8 text
        /// </summary>
        public static string Decode(IntPtr text)
        {
            if (text == IntPtr.Zero)
                return string.Empty;
            byte* p = (byte*)text;
            int length = 0;
            while (p[length] != 0)
                length++;
            return Encoding.GetString(p, length);
        }

        /// <summary>
        /// Read a UTF-8 text allocated by the native library and release it
        /// </summary>
        public static string Take(IntPtr text)
        {
            try
            {
                return Decode(text);
            }
            finally
            {
                if (text != IntPtr.Zero)
                    NativeMethods.ls_free(text);
            }
        }

        /// <summary>
        /// Return the text of UTF-8 bytes
        /// </summary>
        public static string GetString(byte[] bytes)
        {
            return Encoding.GetString(bytes);
        }
    }
}
