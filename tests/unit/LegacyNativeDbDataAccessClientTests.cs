using System.Reflection;

using CInteropSharp.Api.DataAccess;
using CInteropSharp.Api.NativeInterop;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class LegacyNativeDbDataAccessClientTests
{
    private static readonly object Sync = new();
    private static bool IsConfigured;

    [Fact]
    public void Execute_ReturnsRawPayload_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new LegacyNativeDbDataAccessClient();

        RawDbAccessResult result;
        try
        {
            result = client.Execute(new DbAccessRequest(10, 2));
        }
        catch (DatabaseAccessException ex) when (ex.Message.Contains("not configured", StringComparison.OrdinalIgnoreCase))
        {
            // Some CI jobs run unit tests without PostgreSQL connectivity configured for native DB calls.
            return;
        }

        Assert.Equal("legacy-native", result.Source);
        Assert.Equal(1, result.RowCount);
        Assert.Contains("\"purchases\":10", result.Payload, StringComparison.Ordinal);
        Assert.Contains("\"banana\":150", result.Payload, StringComparison.Ordinal);
    }

    [Fact]
    public void Execute_WithNegativeInput_ThrowsDatabaseAccessException_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var client = new LegacyNativeDbDataAccessClient();

        Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(-1, 2)));
    }

    [Fact]
    public void EnsureSuccess_ThrowsMappedMessages()
    {
        var ensureSuccess = typeof(LegacyNativeDbDataAccessClient)
            .GetMethod("EnsureSuccess", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(ensureSuccess);

        AssertStatusMessage(ensureSuccess, NativeStatusCode.InvalidArgument, "invalid input");
        AssertStatusMessage(ensureSuccess, NativeStatusCode.DbNotConfigured, "not configured");
        AssertStatusMessage(ensureSuccess, NativeStatusCode.DbError, "query execution failed");
        AssertStatusMessage(ensureSuccess, NativeStatusCode.InternalError, "Native DB call failed");
    }

    [Fact]
    public void ReadNullTerminatedUtf8_ThrowsWhenPointerIsZero()
    {
        var method = typeof(LegacyNativeDbDataAccessClient)
            .GetMethod("ReadNullTerminatedUtf8", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);

        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object?[] { nint.Zero }));
        var inner = Assert.IsType<DatabaseAccessException>(ex.InnerException);
        Assert.Contains("null payload", inner.Message, StringComparison.OrdinalIgnoreCase);
    }

    private static bool EnsureNativePathConfigured()
    {
        lock (Sync)
        {
            if (IsConfigured)
            {
                return true;
            }

            var root = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "../../../../../"));
            var fileName = NativeLibraryResolver.GetPlatformLibraryName();
            var candidates = new[]
            {
                Path.Combine(root, "build", "native", "bin", "Release"),
                Path.Combine(root, "build", "native", "bin")
            };

            var libraryDir = candidates.FirstOrDefault(path => File.Exists(Path.Combine(path, fileName)));
            if (libraryDir is null)
            {
                return false;
            }

            Environment.SetEnvironmentVariable("BANANA_NATIVE_PATH", libraryDir);

            var configuration = new ConfigurationBuilder().Build();
            using var loggerFactory = LoggerFactory.Create(_ => { });
            var logger = loggerFactory.CreateLogger("LegacyNativeDbDataAccessClientTests");

            try
            {
                NativeLibraryResolver.EnsureConfigured(configuration, logger);
            }
            catch (InvalidOperationException ex) when (ex.Message.Contains("resolver is already set", StringComparison.OrdinalIgnoreCase))
            {
            }

            IsConfigured = true;
            return true;
        }
    }

    private static void AssertStatusMessage(MethodInfo ensureSuccess, NativeStatusCode status, string expectedMessageFragment)
    {
        var exception = Assert.Throws<TargetInvocationException>(() => ensureSuccess.Invoke(null, new object?[] { status }));
        var inner = Assert.IsType<DatabaseAccessException>(exception.InnerException);
        Assert.Contains(expectedMessageFragment, inner.Message, StringComparison.OrdinalIgnoreCase);
    }
}
