using Banana.Api.NativeInterop;

namespace Banana.Api.Tests.Unit.TestDoubles;

public sealed class FakeNativeBananaClient : INativeBananaClient
{
    public NativeStatusCode CalculateBananaStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public int CalculateBananaTotal { get; set; }

    public NativeStatusCode CalculateBananaWithBreakdownStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public int BreakdownTotal { get; set; }
    public int BreakdownBaseAmount { get; set; }
    public int BreakdownBonus { get; set; }

    public NativeStatusCode CreateBananaMessageStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public string CreateBananaMessageValue { get; set; } = string.Empty;

    public NativeStatusCode QueryBananaProfileStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public string QueryBananaProfileJson { get; set; } = string.Empty;

    public NativeStatusCode PredictRegressionStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public double PredictRegressionValue { get; set; }

    public NativeStatusCode ClassifyBananaBinaryStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public string ClassifyBananaBinaryJson { get; set; } = string.Empty;

    public NativeStatusCode ClassifyBananaTransformerStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public string ClassifyBananaTransformerJson { get; set; } = string.Empty;

    public NativeStatusCode ClassifyTransformerWithEmbeddingStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public string ClassifyTransformerWithEmbeddingJson { get; set; } = string.Empty;
    public double[] ClassifyTransformerWithEmbeddingValues { get; set; } = new double[4];
    public int ClassifyTransformerWithEmbeddingCallCount { get; private set; }

    public NativeStatusCode ClassifyNotBananaJunkStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public string ClassifyNotBananaJunkJson { get; set; } = string.Empty;

    public NativeStatusCode PredictBananaRipenessStatus { get; set; } = NativeStatusCode.InvalidArgument;
    public string PredictBananaRipenessJson { get; set; } = string.Empty;

    public NativeStatusCode CreateBatchStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string CreateBatchJson { get; set; } = string.Empty;

    public NativeStatusCode GetBatchStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string GetBatchStatusJson { get; set; } = string.Empty;

    public NativeStatusCode PredictBatchRipenessStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string PredictBatchRipenessJson { get; set; } = string.Empty;

    public NativeStatusCode CreateHarvestBatchStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string CreateHarvestBatchJson { get; set; } = string.Empty;

    public NativeStatusCode AddBunchToHarvestBatchStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string AddBunchToHarvestBatchJson { get; set; } = string.Empty;

    public NativeStatusCode GetHarvestBatchStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string GetHarvestBatchStatusJson { get; set; } = string.Empty;

    public NativeStatusCode RegisterTruckStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string RegisterTruckJson { get; set; } = string.Empty;

    public NativeStatusCode LoadTruckContainerStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string LoadTruckContainerJson { get; set; } = string.Empty;

    public NativeStatusCode UnloadTruckContainerStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string UnloadTruckContainerJson { get; set; } = string.Empty;

    public NativeStatusCode RelocateTruckStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string RelocateTruckJson { get; set; } = string.Empty;

    public NativeStatusCode GetTruckStatusCode { get; set; } = NativeStatusCode.InvalidArgument;
    public string GetTruckStatusJson { get; set; } = string.Empty;

    public NativeStatusCode CalculateBanana(int purchases, int multiplier, out int total)
    {
        total = CalculateBananaTotal;
        return CalculateBananaStatus;
    }

    public NativeStatusCode CalculateBananaWithBreakdown(int purchases, int multiplier, out int total, out int baseAmount, out int bonus)
    {
        total = BreakdownTotal;
        baseAmount = BreakdownBaseAmount;
        bonus = BreakdownBonus;
        return CalculateBananaWithBreakdownStatus;
    }

    public NativeStatusCode CreateBananaMessage(int purchases, int multiplier, out string message)
    {
        message = CreateBananaMessageValue;
        return CreateBananaMessageStatus;
    }

    public NativeStatusCode QueryBananaProfile(string profileId, out string json)
    {
        json = QueryBananaProfileJson;
        return QueryBananaProfileStatus;
    }

    public NativeStatusCode PredictRegressionScore(string inputJson, out double score)
    {
        score = PredictRegressionValue;
        return PredictRegressionStatus;
    }

    public NativeStatusCode ClassifyBananaBinary(string inputJson, out string json)
    {
        json = ClassifyBananaBinaryJson;
        return ClassifyBananaBinaryStatus;
    }

    public NativeStatusCode ClassifyBananaTransformer(string inputJson, out string json)
    {
        json = ClassifyBananaTransformerJson;
        return ClassifyBananaTransformerStatus;
    }

    public NativeStatusCode ClassifyBananaTransformerWithEmbedding(string inputJson, double[] embedding, out string json)
    {
        ClassifyTransformerWithEmbeddingCallCount++;
        ArgumentNullException.ThrowIfNull(embedding);
        if (embedding.Length != 4) throw new ArgumentException("embedding must be 4 doubles", nameof(embedding));
        for (var i = 0; i < 4 && i < ClassifyTransformerWithEmbeddingValues.Length; i++)
        {
            embedding[i] = ClassifyTransformerWithEmbeddingValues[i];
        }
        json = ClassifyTransformerWithEmbeddingJson;
        return ClassifyTransformerWithEmbeddingStatus;
    }

    public NativeStatusCode ClassifyNotBananaJunk(string inputJson, out string json)
    {
        json = ClassifyNotBananaJunkJson;
        return ClassifyNotBananaJunkStatus;
    }

    public NativeStatusCode PredictBananaRipeness(string inputJson, out string json)
    {
        json = PredictBananaRipenessJson;
        return PredictBananaRipenessStatus;
    }

    public NativeStatusCode CreateBatch(string inputJson, out string json)
    {
        json = CreateBatchJson;
        return CreateBatchStatusCode;
    }

    public NativeStatusCode GetBatchStatus(string batchId, out string json)
    {
        json = GetBatchStatusJson;
        return GetBatchStatusCode;
    }

    public NativeStatusCode PredictBatchRipeness(string batchId, out string json)
    {
        json = PredictBatchRipenessJson;
        return PredictBatchRipenessStatusCode;
    }

    public NativeStatusCode CreateHarvestBatch(string inputJson, out string json)
    {
        json = CreateHarvestBatchJson;
        return CreateHarvestBatchStatusCode;
    }

    public NativeStatusCode AddBunchToHarvestBatch(string batchId, string inputJson, out string json)
    {
        json = AddBunchToHarvestBatchJson;
        return AddBunchToHarvestBatchStatusCode;
    }

    public NativeStatusCode GetHarvestBatchStatus(string batchId, out string json)
    {
        json = GetHarvestBatchStatusJson;
        return GetHarvestBatchStatusCode;
    }

    public NativeStatusCode RegisterTruck(string inputJson, out string json)
    {
        json = RegisterTruckJson;
        return RegisterTruckStatusCode;
    }

    public NativeStatusCode LoadTruckContainer(string truckId, string inputJson, out string json)
    {
        json = LoadTruckContainerJson;
        return LoadTruckContainerStatusCode;
    }

    public NativeStatusCode UnloadTruckContainer(string truckId, string containerId, out string json)
    {
        json = UnloadTruckContainerJson;
        return UnloadTruckContainerStatusCode;
    }

    public NativeStatusCode RelocateTruck(string truckId, string inputJson, out string json)
    {
        json = RelocateTruckJson;
        return RelocateTruckStatusCode;
    }

    public NativeStatusCode GetTruckStatus(string truckId, out string json)
    {
        json = GetTruckStatusJson;
        return GetTruckStatusCode;
    }

    // Feature 072 — ABI version stub
    public int FakeVersionMajor { get; set; } = 2;
    public int FakeVersionMinor { get; set; } = 4;
    public NativeStatusCode GetNativeVersion(out int major, out int minor)
    {
        major = FakeVersionMajor;
        minor = FakeVersionMinor;
        return NativeStatusCode.Ok;
    }
}}
