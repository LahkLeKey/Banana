using System.Reflection;

using Banana.Api.NativeInterop;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class NativeBananaClientTests
{
    [Fact]
    public void CallJson_WhenInteropExceptionIsThrown_ReturnsNativeUnavailable()
    {
        var callJsonMethod = typeof(NativeBananaClient).GetMethod(
            "CallJson",
            BindingFlags.NonPublic | BindingFlags.Static);

        Assert.NotNull(callJsonMethod);

        var throwingDelegate = BuildThrowingSingleDelegate();
        object?[] args = ["{}", null, throwingDelegate];

        var raw = callJsonMethod!.Invoke(null, args);

        Assert.NotNull(raw);
        var status = Assert.IsType<NativeStatusCode>(raw);
        Assert.Equal(NativeStatusCode.NativeUnavailable, status);
        Assert.Equal(string.Empty, Assert.IsType<string>(args[1]));
    }

    [Theory]
    [InlineData(typeof(DllNotFoundException), true)]
    [InlineData(typeof(EntryPointNotFoundException), true)]
    [InlineData(typeof(BadImageFormatException), true)]
    [InlineData(typeof(TypeInitializationException), true)]
    [InlineData(typeof(FileNotFoundException), true)]
    [InlineData(typeof(FileLoadException), true)]
    [InlineData(typeof(InvalidOperationException), false)]
    public void IsInteropUnavailable_ClassifiesExpectedExceptions(Type exceptionType, bool expected)
    {
        var method = typeof(NativeBananaClient).GetMethod(
            "IsInteropUnavailable",
            BindingFlags.NonPublic | BindingFlags.Static);

        Assert.NotNull(method);

        var exception = CreateException(exceptionType);
        var raw = method!.Invoke(null, [exception]);

        Assert.NotNull(raw);
        Assert.Equal(expected, Assert.IsType<bool>(raw));
    }

    private static Delegate BuildThrowingSingleDelegate()
    {
        var delegateType = typeof(NativeBananaClient)
            .GetNestedType("Single", BindingFlags.NonPublic);

        Assert.NotNull(delegateType);

        return Delegate.CreateDelegate(delegateType!, typeof(NativeBananaClientTests).GetMethod(
            nameof(ThrowingSingleAdapter),
            BindingFlags.NonPublic | BindingFlags.Static)!);
    }

    private static int ThrowingSingleAdapter(string input, out IntPtr ptr)
    {
        ptr = IntPtr.Zero;
        throw new DllNotFoundException($"native unavailable for input '{input}'");
    }

    private static Exception CreateException(Type exceptionType)
    {
        if (exceptionType == typeof(TypeInitializationException))
        {
            return new TypeInitializationException("NativeMethods", new InvalidOperationException("inner"));
        }

        return (Exception)Activator.CreateInstance(exceptionType, "boom")!;
    }
}
