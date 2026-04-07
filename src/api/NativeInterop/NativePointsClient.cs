using System.Runtime.InteropServices;
using System.Text;

using CInteropSharp.Api.Services;

namespace CInteropSharp.Api.NativeInterop;

public sealed class NativePointsClient : INativePointsClient
{
    public PointsResult Calculate(int purchases, int multiplier)
    {
        var status = (NativeStatusCode)NativeMethods.CalculatePointsWithBreakdown(
            purchases,
            multiplier,
            out var breakdown);

        EnsureSuccess(status);

        status = (NativeStatusCode)NativeMethods.CreatePointsMessage(
            purchases,
            multiplier,
            out var messagePtr);

        EnsureSuccess(status);

        try
        {
            var bytes = ReadNullTerminatedUtf8(messagePtr);
            var message = Encoding.UTF8.GetString(bytes);

            return new PointsResult(
                breakdown.Purchases,
                breakdown.Multiplier,
                breakdown.Points,
                message);
        }
        finally
        {
            NativeMethods.Free(messagePtr);
        }
    }

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

public sealed class NativeInteropException : Exception
{
    public NativeInteropException(string message) : base(message)
    {
    }
}
