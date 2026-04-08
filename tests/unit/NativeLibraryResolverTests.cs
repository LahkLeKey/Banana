using System.Runtime.InteropServices;
using System.Reflection;

using CInteropSharp.Api.NativeInterop;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class NativeLibraryResolverTests
{
    private static readonly ILoggerFactory LoggerFactory = Microsoft.Extensions.Logging.LoggerFactory.Create(_ => { });

    [Fact]
    public void GetPlatformLibraryName_ReturnsExpectedNameForCurrentPlatform()
    {
        var name = NativeLibraryResolver.GetPlatformLibraryName();

        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            Assert.Equal("cinterop_native.dll", name);
            return;
        }

        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            Assert.Equal("libcinterop_native.dylib", name);
            return;
        }

        Assert.Equal("libcinterop_native.so", name);
    }

    [Fact]
    public void GetPlatformLibraryName_WhenWindows_ReturnsDllName()
    {
        var value = NativeLibraryResolver.GetPlatformLibraryName(static os => os == OSPlatform.Windows);
        Assert.Equal("cinterop_native.dll", value);
    }

    [Fact]
    public void GetPlatformLibraryName_WhenOsx_ReturnsDylibName()
    {
        var value = NativeLibraryResolver.GetPlatformLibraryName(static os => os == OSPlatform.OSX);
        Assert.Equal("libcinterop_native.dylib", value);
    }

    [Fact]
    public void GetPlatformLibraryName_WhenNeitherWindowsNorOsx_ReturnsSoName()
    {
        var value = NativeLibraryResolver.GetPlatformLibraryName(static _ => false);
        Assert.Equal("libcinterop_native.so", value);
    }

    [Fact]
    public void ResolveLibrary_WhenLibraryNameDoesNotMatch_ReturnsZero()
    {
        var result = NativeLibraryResolver.ResolveLibrary(
            "different_lib",
            typeof(NativeLibraryResolver).Assembly,
            null,
            new ConfigurationBuilder().Build(),
            CreateLogger(),
            _ => true,
                static (string _, out nint handle) =>
            {
                handle = (nint)123;
                return true;
            },
                static (string _, Assembly _, DllImportSearchPath? _, out nint handle) =>
            {
                handle = (nint)456;
                return true;
            },
            () => "unused.dll");

        Assert.Equal(nint.Zero, result);
    }

    [Fact]
    public void ResolveLibrary_WhenCandidatePathLoads_ReturnsPathHandle()
    {
        const string libraryFile = "cinterop_native.dll";
        var tempDir = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(tempDir);

        try
        {
            Environment.SetEnvironmentVariable("CINTEROP_NATIVE_PATH", tempDir);
            var expectedPath = Path.Combine(tempDir, libraryFile);
            var configuredPath = Path.Combine(tempDir, libraryFile);

            var result = NativeLibraryResolver.ResolveLibrary(
                "cinterop_native",
                typeof(NativeLibraryResolver).Assembly,
                null,
                new ConfigurationBuilder().Build(),
                CreateLogger(),
                path => path == expectedPath,
                (string path, out nint handle) =>
                {
                    handle = (nint)789;
                    return string.Equals(path, configuredPath, StringComparison.Ordinal);
                },
                static (string _, Assembly _, DllImportSearchPath? _, out nint handle) =>
                {
                    handle = nint.Zero;
                    return false;
                },
                () => libraryFile);

            Assert.Equal((nint)789, result);
        }
        finally
        {
            Environment.SetEnvironmentVariable("CINTEROP_NATIVE_PATH", null);
            Directory.Delete(tempDir, true);
        }
    }

    [Fact]
    public void ResolveLibrary_WhenPathLoadFails_UsesDefaultLoader()
    {
        const string libraryFile = "cinterop_native.dll";
        var tempDir = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(tempDir);

        try
        {
            Environment.SetEnvironmentVariable("CINTEROP_NATIVE_PATH", tempDir);
            var expectedPath = Path.Combine(tempDir, libraryFile);

            var result = NativeLibraryResolver.ResolveLibrary(
                "cinterop_native",
                typeof(NativeLibraryResolver).Assembly,
                null,
                new ConfigurationBuilder().Build(),
                CreateLogger(),
                path => path == expectedPath,
                static (string _, out nint handle) =>
                {
                    handle = nint.Zero;
                    return false;
                },
                static (string _, Assembly _, DllImportSearchPath? _, out nint handle) =>
                {
                    handle = (nint)987;
                    return true;
                },
                () => libraryFile);

            Assert.Equal((nint)987, result);
        }
        finally
        {
            Environment.SetEnvironmentVariable("CINTEROP_NATIVE_PATH", null);
            Directory.Delete(tempDir, true);
        }
    }

    [Fact]
    public void ResolveLibrary_WhenNoCandidateOrDefaultLoad_ReturnsZero()
    {
        var result = NativeLibraryResolver.ResolveLibrary(
            "cinterop_native",
            typeof(NativeLibraryResolver).Assembly,
            null,
            new ConfigurationBuilder().Build(),
            CreateLogger(),
            _ => false,
            static (string _, out nint handle) =>
            {
                handle = nint.Zero;
                return false;
            },
            static (string _, Assembly _, DllImportSearchPath? _, out nint handle) =>
            {
                handle = nint.Zero;
                return false;
            },
            () => "cinterop_native.dll");

        Assert.Equal(nint.Zero, result);
    }

    [Fact]
    public void EnsureConfigured_CanBeCalledMultipleTimes()
    {
        var configuration = new ConfigurationBuilder().Build();
        var logger = CreateLogger();

        NativeLibraryResolver.EnsureConfigured(configuration, logger);
        NativeLibraryResolver.EnsureConfigured(configuration, logger);
    }

    private static ILogger CreateLogger()
    {
        return LoggerFactory.CreateLogger("NativeLibraryResolverTests");
    }
}
