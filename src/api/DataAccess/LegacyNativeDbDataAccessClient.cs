using System.Runtime.InteropServices;
using System.Text;

using CInteropSharp.Api.NativeInterop;

namespace CInteropSharp.Api.DataAccess;

/// <summary>
/// Default data-access mode that preserves legacy native behavior behind the DB stage boundary.
/// </summary>
public sealed class LegacyNativeDbDataAccessClient : IDataAccessPipelineClient
{
    /// <inheritdoc />
    public RawDbAccessResult Execute(DbAccessRequest request)
    {
        var status = (NativeStatusCode)NativeMethods.QueryDbPoints(
            request.Purchases,
            request.Multiplier,
            out var payloadPtr,
            out var rowCount);

        EnsureSuccess(status);

        try
        {
            var payload = Encoding.UTF8.GetString(ReadNullTerminatedUtf8(payloadPtr));
            return new RawDbAccessResult("legacy-native", payload, rowCount);
        }
        finally
        {
            NativeMethods.Free(payloadPtr);
        }
    }

    private static byte[] ReadNullTerminatedUtf8(nint pointer)
    {
        if (pointer == nint.Zero)
        {
            throw new DatabaseAccessException("Native DB call returned null payload.");
        }

        var bytes = new List<byte>();
        var offset = 0;
        while (true)
        {
            var value = Marshal.ReadByte(pointer, offset);
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
            NativeStatusCode.InvalidArgument => new DatabaseAccessException("Native DB call rejected invalid input."),
            NativeStatusCode.DbNotConfigured => new DatabaseAccessException("Native DB support is not configured."),
            NativeStatusCode.DbError => new DatabaseAccessException("Native DB query execution failed."),
            _ => new DatabaseAccessException($"Native DB call failed: {status}")
        };
    }
}
