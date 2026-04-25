using System.Runtime.InteropServices;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Library resolution: BANANA_NATIVE_PATH env var prepends the search path.
/// (Spec 007 contract.) Uses LibraryImport (source-generated P/Invoke).
/// </summary>
internal static partial class NativeMethods
{
    public const string LibraryName = "banana_native";

    static NativeMethods()
    {
        var nativePath = Environment.GetEnvironmentVariable("BANANA_NATIVE_PATH");
        if (!string.IsNullOrEmpty(nativePath))
        {
            NativeLibrary.SetDllImportResolver(typeof(NativeMethods).Assembly, (name, _, _) =>
            {
                if (name != LibraryName) return IntPtr.Zero;
                foreach (var candidate in new[]
                {
                    Path.Combine(nativePath, $"lib{LibraryName}.so"),
                    Path.Combine(nativePath, $"{LibraryName}.dll"),
                    Path.Combine(nativePath, $"{LibraryName}.dylib"),
                })
                {
                    if (File.Exists(candidate) && NativeLibrary.TryLoad(candidate, out var handle))
                        return handle;
                }
                return IntPtr.Zero;
            });
        }
    }

    [LibraryImport(LibraryName, EntryPoint = "banana_calculate_banana")]
    public static partial int CalculateBanana(int purchases, int multiplier, out int outBanana);

    [LibraryImport(LibraryName, EntryPoint = "banana_calculate_banana_with_breakdown")]
    public static partial int CalculateBananaWithBreakdown(int purchases, int multiplier, out int outBanana, out int outBase, out int outBonus);

    [LibraryImport(LibraryName, EntryPoint = "banana_create_banana_message")]
    public static partial int CreateBananaMessage(int purchases, int multiplier, out IntPtr outMessage);

    [LibraryImport(LibraryName, EntryPoint = "banana_db_query_banana_profile", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int QueryBananaProfile(string profileId, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_predict_banana_regression_score", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int PredictRegressionScore(string inputJson, out double outScore);

    [LibraryImport(LibraryName, EntryPoint = "banana_classify_banana_binary", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int ClassifyBananaBinary(string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_classify_banana_transformer", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int ClassifyBananaTransformer(string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_classify_not_banana_junk", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int ClassifyNotBananaJunk(string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_predict_banana_ripeness", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int PredictBananaRipeness(string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_create_batch", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int CreateBatch(string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_get_batch_status", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int GetBatchStatus(string batchId, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_predict_batch_ripeness", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int PredictBatchRipeness(string batchId, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_create_harvest_batch", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int CreateHarvestBatch(string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_add_bunch_to_harvest_batch", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int AddBunchToHarvestBatch(string batchId, string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_get_harvest_batch_status", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int GetHarvestBatchStatus(string batchId, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_register_truck", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int RegisterTruck(string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_load_truck_container", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int LoadTruckContainer(string truckId, string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_unload_truck_container", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int UnloadTruckContainer(string truckId, string containerId, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_relocate_truck", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int RelocateTruck(string truckId, string inputJson, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_get_truck_status", StringMarshalling = StringMarshalling.Utf8)]
    public static partial int GetTruckStatus(string truckId, out IntPtr outJson);

    [LibraryImport(LibraryName, EntryPoint = "banana_free")]
    public static partial void Free(IntPtr pointer);
}
