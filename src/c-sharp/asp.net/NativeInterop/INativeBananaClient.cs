namespace Banana.Api.NativeInterop;

/// <summary>
/// Single interop seam (spec 007). Adding a method MUST NOT require updating
/// more than 2 test files. Fakes use the abstract base or a generated impl.
/// </summary>
public interface INativeBananaClient
{
    NativeStatusCode CalculateBanana(int purchases, int multiplier, out int total);
    NativeStatusCode CalculateBananaWithBreakdown(int purchases, int multiplier, out int total, out int baseAmount, out int bonus);
    NativeStatusCode CreateBananaMessage(int purchases, int multiplier, out string message);

    NativeStatusCode QueryBananaProfile(string profileId, out string json);

    NativeStatusCode PredictRegressionScore(string inputJson, out double score);
    NativeStatusCode ClassifyBananaBinary(string inputJson, out string json);
    NativeStatusCode ClassifyBananaTransformer(string inputJson, out string json);
    NativeStatusCode ClassifyNotBananaJunk(string inputJson, out string json);
    NativeStatusCode PredictBananaRipeness(string inputJson, out string json);

    NativeStatusCode CreateBatch(string inputJson, out string json);
    NativeStatusCode GetBatchStatus(string batchId, out string json);
    NativeStatusCode PredictBatchRipeness(string batchId, out string json);

    NativeStatusCode CreateHarvestBatch(string inputJson, out string json);
    NativeStatusCode AddBunchToHarvestBatch(string batchId, string inputJson, out string json);
    NativeStatusCode GetHarvestBatchStatus(string batchId, out string json);

    NativeStatusCode RegisterTruck(string inputJson, out string json);
    NativeStatusCode LoadTruckContainer(string truckId, string inputJson, out string json);
    NativeStatusCode UnloadTruckContainer(string truckId, string containerId, out string json);
    NativeStatusCode RelocateTruck(string truckId, string inputJson, out string json);
    NativeStatusCode GetTruckStatus(string truckId, out string json);
}
