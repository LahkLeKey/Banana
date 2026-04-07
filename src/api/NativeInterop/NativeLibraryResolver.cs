using System.Reflection;
using System.Runtime.InteropServices;

namespace CInteropSharp.Api.NativeInterop;

public static class NativeLibraryResolver
{
    private static bool _isConfigured;

    public static void EnsureConfigured(IConfiguration configuration, ILogger logger)
    {
        if (_isConfigured)
        {
            return;
        }

        var assembly = typeof(NativeMethods).Assembly;

        NativeLibrary.SetDllImportResolver(assembly, (libraryName, _, searchPath) =>
        {
            if (!string.Equals(libraryName, "cinterop_native", StringComparison.Ordinal))
            {
                return nint.Zero;
            }

            var candidateDirectories = new[]
            {
                Environment.GetEnvironmentVariable("CINTEROP_NATIVE_PATH"),
                configuration["Native:LibraryPath"],
                AppContext.BaseDirectory
            };

            foreach (var dir in candidateDirectories)
            {
                if (string.IsNullOrWhiteSpace(dir))
                {
                    continue;
                }

                var fullPath = Path.Combine(dir, GetPlatformLibraryName());
                if (File.Exists(fullPath) && NativeLibrary.TryLoad(fullPath, out var handle))
                {
                    logger.LogInformation("Loaded native library from {Path}", fullPath);
                    return handle;
                }
            }

            if (NativeLibrary.TryLoad(libraryName, assembly, searchPath, out var defaultHandle))
            {
                return defaultHandle;
            }

            return nint.Zero;
        });

        _isConfigured = true;
    }

    public static string GetPlatformLibraryName()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            return "cinterop_native.dll";
        }

        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            return "libcinterop_native.dylib";
        }

        return "libcinterop_native.so";
    }
}
