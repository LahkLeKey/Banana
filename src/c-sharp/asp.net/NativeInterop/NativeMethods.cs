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
    /// Executes native DB stage query and returns UTF-8 JSON payload allocated by native code.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_db_query_banana")]
    internal static partial int QueryDbBanana(int purchases, int multiplier, out nint payload, out int rowCount);

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
