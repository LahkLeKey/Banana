using System.Runtime.InteropServices;
using System.Text;

using CInteropSharp.Api.Services;

namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Concrete interop client that calls exported C wrapper functions via P/Invoke.
/// </summary>
public sealed class NativeBananaClient : INativeBananaClient
{
    /// <inheritdoc />
    public BananaResult Calculate(int purchases, int multiplier)
    {
        var status = (NativeStatusCode)NativeMethods.CalculateBananaWithBreakdown(
            purchases,
            multiplier,
            out var breakdown);

        EnsureSuccess(status);

        status = (NativeStatusCode)NativeMethods.CreateBananaMessage(
            purchases,
            multiplier,
            out var messagePtr);

        EnsureSuccess(status);

        try
        {
            var bytes = ReadNullTerminatedUtf8(messagePtr);
            var message = Encoding.UTF8.GetString(bytes);

            return new BananaResult(
                breakdown.Purchases,
                breakdown.Multiplier,
                breakdown.Banana,
                message);
        }
        finally
        {
            NativeMethods.Free(messagePtr);
        }
    }

    /// <summary>
    /// Reads a UTF-8 null-terminated native string from unmanaged memory.
    /// </summary>
    /// <param name="messagePtr">Pointer returned from native code.</param>
    /// <returns>Raw UTF-8 bytes without the terminating zero byte.</returns>
    private static byte[] ReadNullTerminatedUtf8(nint messagePtr)
    {
        if (messagePtr == nint.Zero)
        {
            throw new NativeInteropException("Native returned null message pointer.");
        }

        var bytes = new List<byte>();
        var offset = 0;

        while (true)
        {
            var value = Marshal.ReadByte(messagePtr, offset);
            if (value == 0)
            {
                break;
            }

            bytes.Add(value);
            offset++;
        }

        return bytes.ToArray();
    }

    /// <summary>
    /// Maps native status codes to managed exceptions.
    /// </summary>
    /// <param name="status">Native return status.</param>
    private static void EnsureSuccess(NativeStatusCode status)
    {
        if (status == NativeStatusCode.Ok)
        {
            return;
        }

        throw status switch
        {
            NativeStatusCode.InvalidArgument => new ClientInputException("Native validation failed for input values."),
            NativeStatusCode.Overflow => new ClientInputException("Input values are too large for native processing."),
            _ => new NativeInteropException($"Native call failed: {status}")
        };
    }
}

/// <summary>
/// Represents failures that occur at the native interop boundary.
/// </summary>
public sealed class NativeInteropException : Exception
{
    /// <summary>
    /// Initializes a new instance of the <see cref="NativeInteropException"/> class.
    /// </summary>
    /// <param name="message">Failure details for diagnostics.</param>
    public NativeInteropException(string message) : base(message)
    {
    }
}
