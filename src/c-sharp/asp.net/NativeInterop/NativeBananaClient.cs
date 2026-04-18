using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;

using Banana.Api.Services;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Concrete interop client that calls exported C wrapper functions via P/Invoke.
/// </summary>
public sealed class NativeBananaClient : INativeBananaClient
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true
    };

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

    /// <inheritdoc />
    public BananaBatchRecord CreateBatch(
        string batchId,
        string originFarm,
        double storageTempC,
        double ethyleneExposure,
        int estimatedShelfLifeDays)
    {
        var status = (NativeStatusCode)NativeMethods.CreateBatch(
            batchId,
            originFarm,
            storageTempC,
            ethyleneExposure,
            estimatedShelfLifeDays,
            out var batchJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeBatchRecord(batchJsonPtr);
        }
        finally
        {
            NativeMethods.Free(batchJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaBatchRecord GetBatchStatus(string batchId)
    {
        var status = (NativeStatusCode)NativeMethods.GetBatchStatus(batchId, out var batchJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeBatchRecord(batchJsonPtr);
        }
        finally
        {
            NativeMethods.Free(batchJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaRipenessPrediction PredictRipeness(
        IReadOnlyList<double> temperatureHistoryC,
        int daysSinceHarvest,
        double ethyleneExposure,
        double mechanicalDamage,
        double storageTempC)
    {
        var history = temperatureHistoryC?.ToArray() ?? [];
        var status = (NativeStatusCode)NativeMethods.PredictBananaRipeness(
            history,
            history.Length,
            daysSinceHarvest,
            ethyleneExposure,
            mechanicalDamage,
            storageTempC,
            out var prediction);

        EnsureSuccess(status);

        return new BananaRipenessPrediction(
            MapStageName((BananaRipenessStage)prediction.PredictedStage),
            prediction.ShelfLifeHours,
            prediction.RipeningIndex,
            prediction.SpoilageProbability,
            prediction.ColdChainRisk);
    }

    /// <inheritdoc />
    public BananaRipenessPrediction PredictBatchRipeness(
        string batchId,
        IReadOnlyList<double> temperatureHistoryC,
        int daysSinceHarvest,
        double mechanicalDamage)
    {
        var history = temperatureHistoryC?.ToArray() ?? [];
        var status = (NativeStatusCode)NativeMethods.PredictBatchRipeness(
            batchId,
            history,
            history.Length,
            daysSinceHarvest,
            mechanicalDamage,
            out var prediction);

        EnsureSuccess(status);

        return new BananaRipenessPrediction(
            MapStageName((BananaRipenessStage)prediction.PredictedStage),
            prediction.ShelfLifeHours,
            prediction.RipeningIndex,
            prediction.SpoilageProbability,
            prediction.ColdChainRisk);
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

    private static BananaBatchRecord DeserializeBatchRecord(nint batchJsonPtr)
    {
        var json = Encoding.UTF8.GetString(ReadNullTerminatedUtf8(batchJsonPtr));
        return JsonSerializer.Deserialize<BananaBatchRecord>(json, JsonOptions)
            ?? throw new NativeInteropException("Native returned an invalid batch payload.");
    }

    private static string MapStageName(BananaRipenessStage stage)
    {
        return stage switch
        {
            BananaRipenessStage.Green => "GREEN",
            BananaRipenessStage.Breaking => "BREAKING",
            BananaRipenessStage.Yellow => "YELLOW",
            BananaRipenessStage.Spotted => "SPOTTED",
            BananaRipenessStage.Overripe => "OVERRIPE",
            BananaRipenessStage.Biodegradation => "BIODEGRADATION",
            _ => throw new NativeInteropException($"Native returned unknown ripeness stage: {(int)stage}")
        };
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
            NativeStatusCode.NotFound => new EntityNotFoundException("Native resource was not found."),
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
