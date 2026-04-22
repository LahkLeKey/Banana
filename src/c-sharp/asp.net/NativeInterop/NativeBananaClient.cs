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

    private const int BananaMlFeatureCount = 8;
    private const int BananaMlTokenFeatureCount = 4;
    private const int BananaMlMaxSequenceLength = 16;

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
    public double PredictBananaRegressionScore(IReadOnlyList<double> features)
    {
        var normalized = NormalizeFeatureVector(features);
        var status = (NativeStatusCode)NativeMethods.PredictBananaRegressionScore(
            normalized,
            normalized.Length,
            out var score);

        EnsureSuccess(status);
        return score;
    }

    /// <inheritdoc />
    public BananaMlBinaryClassification PredictBananaBinaryClassification(IReadOnlyList<double> features)
    {
        var normalized = NormalizeFeatureVector(features);
        var status = (NativeStatusCode)NativeMethods.ClassifyBananaBinary(
            normalized,
            normalized.Length,
            out var classification);

        EnsureSuccess(status);

        return new BananaMlBinaryClassification(
            MapMlLabelName(classification.PredictedLabel),
            classification.BananaProbability,
            classification.NotBananaProbability,
            classification.DecisionMargin);
    }

    /// <inheritdoc />
    public BananaMlTransformerClassification PredictBananaTransformerClassification(IReadOnlyList<double> tokenFeatures)
    {
        var normalized = NormalizeTransformerTokenValues(tokenFeatures);
        var status = (NativeStatusCode)NativeMethods.ClassifyBananaTransformer(
            normalized,
            normalized.Length,
            out var classification);

        EnsureSuccess(status);

        return new BananaMlTransformerClassification(
            MapMlLabelName(classification.PredictedLabel),
            classification.BananaProbability,
            classification.NotBananaProbability,
            classification.AttentionFocus);
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
            return DeserializeRecord<BananaBatchRecord>(batchJsonPtr, "Native returned an invalid batch payload.");
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
            return DeserializeRecord<BananaBatchRecord>(batchJsonPtr, "Native returned an invalid batch payload.");
        }
        finally
        {
            NativeMethods.Free(batchJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaHarvestBatchRecord CreateHarvestBatch(
        string harvestBatchId,
        string fieldId,
        int harvestDayOrdinal)
    {
        var status = (NativeStatusCode)NativeMethods.CreateHarvestBatch(
            harvestBatchId,
            fieldId,
            harvestDayOrdinal,
            out var harvestBatchJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaHarvestBatchRecord>(
                harvestBatchJsonPtr,
                "Native returned an invalid harvest batch payload.");
        }
        finally
        {
            NativeMethods.Free(harvestBatchJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaHarvestBatchRecord AddBunchToHarvestBatch(
        string harvestBatchId,
        string bunchId,
        int harvestDayOrdinal,
        double bunchWeightKg)
    {
        var status = (NativeStatusCode)NativeMethods.AddBunchToHarvestBatch(
            harvestBatchId,
            bunchId,
            harvestDayOrdinal,
            bunchWeightKg,
            out var harvestBatchJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaHarvestBatchRecord>(
                harvestBatchJsonPtr,
                "Native returned an invalid harvest batch payload.");
        }
        finally
        {
            NativeMethods.Free(harvestBatchJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaHarvestBatchRecord GetHarvestBatchStatus(string harvestBatchId)
    {
        var status = (NativeStatusCode)NativeMethods.GetHarvestBatchStatus(harvestBatchId, out var harvestBatchJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaHarvestBatchRecord>(
                harvestBatchJsonPtr,
                "Native returned an invalid harvest batch payload.");
        }
        finally
        {
            NativeMethods.Free(harvestBatchJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaTruckRecord RegisterTruck(
        string truckId,
        string nodeId,
        BananaDistributionNodeType nodeType,
        double latitude,
        double longitude,
        double capacityKg)
    {
        var status = (NativeStatusCode)NativeMethods.RegisterTruck(
            truckId,
            nodeId,
            nodeType,
            latitude,
            longitude,
            capacityKg,
            out var truckJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaTruckRecord>(truckJsonPtr, "Native returned an invalid truck payload.");
        }
        finally
        {
            NativeMethods.Free(truckJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaTruckRecord LoadTruckContainer(
        string truckId,
        string containerId,
        double containerWeightKg)
    {
        var status = (NativeStatusCode)NativeMethods.LoadTruckContainer(
            truckId,
            containerId,
            containerWeightKg,
            out var truckJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaTruckRecord>(truckJsonPtr, "Native returned an invalid truck payload.");
        }
        finally
        {
            NativeMethods.Free(truckJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaTruckRecord UnloadTruckContainer(
        string truckId,
        string containerId,
        double containerWeightKg)
    {
        var status = (NativeStatusCode)NativeMethods.UnloadTruckContainer(
            truckId,
            containerId,
            containerWeightKg,
            out var truckJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaTruckRecord>(truckJsonPtr, "Native returned an invalid truck payload.");
        }
        finally
        {
            NativeMethods.Free(truckJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaTruckRecord RelocateTruck(
        string truckId,
        string nodeId,
        BananaDistributionNodeType nodeType,
        double latitude,
        double longitude)
    {
        var status = (NativeStatusCode)NativeMethods.RelocateTruck(
            truckId,
            nodeId,
            nodeType,
            latitude,
            longitude,
            out var truckJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaTruckRecord>(truckJsonPtr, "Native returned an invalid truck payload.");
        }
        finally
        {
            NativeMethods.Free(truckJsonPtr);
        }
    }

    /// <inheritdoc />
    public BananaTruckRecord GetTruckStatus(string truckId)
    {
        var status = (NativeStatusCode)NativeMethods.GetTruckStatus(truckId, out var truckJsonPtr);

        EnsureSuccess(status);

        try
        {
            return DeserializeRecord<BananaTruckRecord>(truckJsonPtr, "Native returned an invalid truck payload.");
        }
        finally
        {
            NativeMethods.Free(truckJsonPtr);
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
            MapStageName(prediction.PredictedStage),
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
            MapStageName(prediction.PredictedStage),
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

    private static double[] NormalizeFeatureVector(IReadOnlyList<double> features)
    {
        if (features is null)
        {
            throw new ClientInputException("features are required.");
        }

        if (features.Count != BananaMlFeatureCount)
        {
            throw new ClientInputException($"features must contain exactly {BananaMlFeatureCount} values.");
        }

        var normalized = new double[BananaMlFeatureCount];
        for (var index = 0; index < BananaMlFeatureCount; index++)
        {
            if (!double.IsFinite(features[index]))
            {
                throw new ClientInputException("features must contain finite numeric values.");
            }

            normalized[index] = features[index];
        }

        return normalized;
    }

    private static double[] NormalizeTransformerTokenValues(IReadOnlyList<double> tokenFeatures)
    {
        if (tokenFeatures is null)
        {
            throw new ClientInputException("tokenFeatures are required.");
        }

        if (tokenFeatures.Count == 0)
        {
            throw new ClientInputException("tokenFeatures must contain at least one token.");
        }

        if ((tokenFeatures.Count % BananaMlTokenFeatureCount) != 0)
        {
            throw new ClientInputException($"tokenFeatures must be divisible by {BananaMlTokenFeatureCount} values per token.");
        }

        var tokenCount = tokenFeatures.Count / BananaMlTokenFeatureCount;
        if (tokenCount > BananaMlMaxSequenceLength)
        {
            throw new ClientInputException($"tokenFeatures exceed the maximum sequence length of {BananaMlMaxSequenceLength}.");
        }

        var normalized = new double[tokenFeatures.Count];
        for (var index = 0; index < tokenFeatures.Count; index++)
        {
            if (!double.IsFinite(tokenFeatures[index]))
            {
                throw new ClientInputException("tokenFeatures must contain finite numeric values.");
            }

            normalized[index] = tokenFeatures[index];
        }

        return normalized;
    }

    private static TRecord DeserializeRecord<TRecord>(nint payloadPtr, string invalidPayloadMessage)
    {
        var json = Encoding.UTF8.GetString(ReadNullTerminatedUtf8(payloadPtr));
        return JsonSerializer.Deserialize<TRecord>(json, JsonOptions)
            ?? throw new NativeInteropException(invalidPayloadMessage);
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

    private static string MapMlLabelName(BananaMlLabel label)
    {
        return label switch
        {
            BananaMlLabel.Banana => "BANANA",
            BananaMlLabel.NotBanana => "NOT_BANANA",
            _ => throw new NativeInteropException($"Native returned unknown banana model label: {(int)label}")
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
