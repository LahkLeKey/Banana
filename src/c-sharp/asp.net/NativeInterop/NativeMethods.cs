using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Low-level P/Invoke signatures exposed by the C wrapper library.
/// </summary>
internal static partial class NativeMethods
{
    private const string LibraryName = "banana_native";

    /// <summary>
    /// Calls native calculation and returns banana only.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_calculate_banana")]
    internal static partial int CalculateBanana(int purchases, int multiplier, out int banana);

    /// <summary>
    /// Calls native calculation and returns a detailed breakdown struct.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_calculate_banana_with_breakdown")]
    internal static partial int CalculateBananaWithBreakdown(int purchases, int multiplier, out BananaBreakdownNative breakdown);

    /// <summary>
    /// Creates a native message string that must be released by <see cref="Free"/>.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_create_banana_message")]
    internal static partial int CreateBananaMessage(int purchases, int multiplier, out nint message);

    /// <summary>
    /// Executes the native banana profile projection and returns a UTF-8 JSON payload allocated by native code.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_db_query_banana_profile")]
    internal static partial int QueryBananaProfileProjection(int purchases, int multiplier, out nint payload, out int rowCount);

    /// <summary>
    /// Predicts a bounded banana regression score from a fixed-size feature vector.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_predict_banana_regression_score")]
    internal static partial int PredictBananaRegressionScore(
        [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] double[] features,
        int featureCount,
        out double score);

    /// <summary>
    /// Classifies banana vs non-banana for a fixed-size feature vector.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_classify_banana_binary")]
    internal static partial int ClassifyBananaBinary(
        [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] double[] features,
        int featureCount,
        out BananaMlBinaryClassificationNative classification);

    /// <summary>
    /// Classifies banana vs non-banana for a flattened transformer token feature sequence.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_classify_banana_transformer")]
    internal static partial int ClassifyBananaTransformer(
        [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] double[] tokenFeatures,
        int tokenFeatureValueCount,
        out BananaMlTransformerClassificationNative classification);

    /// <summary>
    /// Predicts banana ripeness from supply-chain telemetry.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_predict_banana_ripeness")]
    internal static partial int PredictBananaRipeness(
        [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] double[] temperatureHistoryC,
        int temperatureHistoryCount,
        int daysSinceHarvest,
        double ethyleneExposure,
        double mechanicalDamage,
        double storageTempC,
        out BananaRipenessPredictionNative prediction);

    /// <summary>
    /// Creates or updates a tracked banana batch in native memory and returns a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_create_batch", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int CreateBatch(
        string batchId,
        string originFarm,
        double storageTempC,
        double ethyleneExposure,
        int estimatedShelfLifeDays,
        out nint batchJson);

    /// <summary>
    /// Retrieves a tracked banana batch as a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_get_batch_status", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int GetBatchStatus(string batchId, out nint batchJson);

    /// <summary>
    /// Creates a tracked harvest batch and returns a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_create_harvest_batch", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int CreateHarvestBatch(
        string harvestBatchId,
        string fieldId,
        int harvestDayOrdinal,
        out nint harvestBatchJson);

    /// <summary>
    /// Adds a bunch to a tracked harvest batch and returns a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_add_bunch_to_harvest_batch", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int AddBunchToHarvestBatch(
        string harvestBatchId,
        string bunchId,
        int harvestDayOrdinal,
        double bunchWeightKg,
        out nint harvestBatchJson);

    /// <summary>
    /// Retrieves a tracked harvest batch as a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_get_harvest_batch_status", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int GetHarvestBatchStatus(string harvestBatchId, out nint harvestBatchJson);

    /// <summary>
    /// Registers a tracked truck and returns a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_register_truck", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int RegisterTruck(
        string truckId,
        string nodeId,
        BananaDistributionNodeType nodeType,
        double latitude,
        double longitude,
        double capacityKg,
        out nint truckJson);

    /// <summary>
    /// Loads a container onto a tracked truck and returns a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_load_truck_container", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int LoadTruckContainer(
        string truckId,
        string containerId,
        double containerWeightKg,
        out nint truckJson);

    /// <summary>
    /// Unloads a container from a tracked truck and returns a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_unload_truck_container", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int UnloadTruckContainer(
        string truckId,
        string containerId,
        double containerWeightKg,
        out nint truckJson);

    /// <summary>
    /// Relocates a tracked truck and returns a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_relocate_truck", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int RelocateTruck(
        string truckId,
        string nodeId,
        BananaDistributionNodeType nodeType,
        double latitude,
        double longitude,
        out nint truckJson);

    /// <summary>
    /// Retrieves a tracked truck as a JSON snapshot.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_get_truck_status", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int GetTruckStatus(string truckId, out nint truckJson);

    /// <summary>
    /// Predicts ripeness for a tracked native batch using stored telemetry defaults.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_predict_batch_ripeness", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial int PredictBatchRipeness(
        string batchId,
        [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] double[] temperatureHistoryC,
        int temperatureHistoryCount,
        int daysSinceHarvest,
        double mechanicalDamage,
        out BananaRipenessPredictionNative prediction);

    /// <summary>
    /// Releases unmanaged memory allocated by the native wrapper.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_free")]
    internal static partial void Free(nint pointer);
}
