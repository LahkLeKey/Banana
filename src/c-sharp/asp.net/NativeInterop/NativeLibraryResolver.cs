using System.Reflection;
using System.Runtime.InteropServices;

namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Configures and executes runtime resolution of the native library used by P/Invoke.
/// </summary>
public static class NativeLibraryResolver
{
    /// <summary>
    /// Delegate used to load a native library from an explicit full path.
    /// </summary>
    internal delegate bool TryLoadPathDelegate(string libraryPath, out nint handle);

    /// <summary>
    /// Delegate used to load a native library using default runtime search rules.
    /// </summary>
    internal delegate bool TryLoadDefaultDelegate(string libraryName, Assembly assembly, DllImportSearchPath? searchPath, out nint handle);

    private static bool _isConfigured;

    /// <summary>
    /// Registers a resolver for the native library one time per process.
    /// </summary>
    /// <param name="configuration">Application configuration used to read optional custom library paths.</param>
    /// <param name="logger">Logger used to record successful native library discovery.</param>
    public static void EnsureConfigured(IConfiguration configuration, ILogger logger)
    {
        if (_isConfigured)
        {
            return;
        }

        var assembly = typeof(NativeMethods).Assembly;

        try
        {
            NativeLibrary.SetDllImportResolver(assembly, (libraryName, _, searchPath) =>
                ResolveLibrary(
                    libraryName,
                    assembly,
                    searchPath,
                    configuration,
                    logger,
                    File.Exists,
                    NativeLibrary.TryLoad,
                    NativeLibrary.TryLoad,
                    GetPlatformLibraryName));
        }
        catch (InvalidOperationException ex) when (ex.Message.Contains("resolver is already set", StringComparison.OrdinalIgnoreCase))
        {
            logger.LogDebug("Native library resolver was already configured for this assembly.");
        }

        _isConfigured = true;
    }

    /// <summary>
    /// Resolves a native library handle by checking configured directories first, then default runtime loading.
    /// </summary>
    /// <remarks>
    /// This method is internal to keep behavior testable without requiring platform-specific integration setup.
    /// </remarks>
    internal static nint ResolveLibrary(
        string libraryName,
        Assembly assembly,
        DllImportSearchPath? searchPath,
        IConfiguration configuration,
        ILogger logger,
        Func<string, bool> fileExists,
        TryLoadPathDelegate tryLoadPath,
        TryLoadDefaultDelegate tryLoadDefault,
        Func<string> getPlatformLibraryName)
    {
        if (!string.Equals(libraryName, "banana_native", StringComparison.Ordinal))
        {
            return nint.Zero;
        }

        var candidateDirectories = new[]
        {
            Environment.GetEnvironmentVariable("BANANA_NATIVE_PATH"),
            configuration["Native:LibraryPath"],
            AppContext.BaseDirectory
        };

        foreach (var dir in candidateDirectories)
        {
            if (string.IsNullOrWhiteSpace(dir))
            {
                continue;
            }

            var fullPath = Path.Combine(dir, getPlatformLibraryName());
            if (fileExists(fullPath) && tryLoadPath(fullPath, out var handle))
            {
                logger.LogInformation("Loaded native library from {Path}", fullPath);
                return handle;
            }
        }

        if (tryLoadDefault(libraryName, assembly, searchPath, out var defaultHandle))
        {
            return defaultHandle;
        }

        return nint.Zero;
    }

    /// <summary>
    /// Gets the platform-specific library file name for the current operating system.
    /// </summary>
    public static string GetPlatformLibraryName()
    {
        return GetPlatformLibraryName(RuntimeInformation.IsOSPlatform);
    }

    /// <summary>
    /// Gets the platform-specific library file name using a supplied OS probe function.
    /// </summary>
    /// <param name="isOSPlatform">Function used to determine which OS platform is active.</param>
    /// <returns>Native library file name for the active platform.</returns>
    internal static string GetPlatformLibraryName(Func<OSPlatform, bool> isOSPlatform)
    {
        if (isOSPlatform(OSPlatform.Windows))
        {
            return "banana_native.dll";
        }

        if (isOSPlatform(OSPlatform.OSX))
        {
            return "libbanana_native.dylib";
        }

        return "libbanana_native.so";
    }
}
