using System.Runtime.InteropServices;
using System.Text;

using Banana.Api.NativeInterop;

namespace Banana.Api.DataAccess;

/// <summary>
/// Default data-access mode that uses the native DAL behind the DB stage boundary.
/// </summary>
public sealed class NativeDalDbDataAccessClient : IDataAccessPipelineClient
{
    /// <inheritdoc />
    public RawDbAccessResult Execute(DbAccessRequest request)
    {
        var (status, payloadPtr, rowCount) = request.Contract switch
        {
            DbAccessContract.BananaProfileProjection => ExecuteBananaProfileProjection(request),
            _ => throw new DatabaseAccessException($"Native DAL does not expose contract '{request.Contract}'.")
        };

        EnsureSuccess(status);

        try
        {
            var payload = Encoding.UTF8.GetString(ReadNullTerminatedUtf8(payloadPtr));
            return new RawDbAccessResult("native-dal", payload, rowCount);
        }
        finally
        {
            NativeMethods.Free(payloadPtr);
        }
    }

    private static (NativeStatusCode Status, nint PayloadPtr, int RowCount) ExecuteBananaProfileProjection(DbAccessRequest request)
    {
        var status = (NativeStatusCode)NativeMethods.QueryBananaProfileProjection(
            request.Purchases,
            request.Multiplier,
            out var payloadPtr,
            out var rowCount);

        return (status, payloadPtr, rowCount);
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
