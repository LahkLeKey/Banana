using System.Globalization;

using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline.Results;
using Banana.Api.Pipeline.Steps;
using Banana.Api.Tests.Unit.TestDoubles;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.Api.Tests.Unit;

/// <summary>
/// Slice 017 -- coverage of the additive `/ml/ensemble/embedding` route.
/// Validates the 6 SPIKE anchor walks (3 cheap-path -> embedding=null,
/// 3 escalated -> embedding length=4) plus the legacy `/ml/ensemble`
/// shape lock and the cheap-path no-call invariant.
/// </summary>
public sealed class EnsembleEmbeddingPassthroughTests
{
    private static readonly double[] StubEmbedding = [0.42, -0.11, 0.07, 0.83];

    public static TheoryData<string, string, double, bool> CheapPathAnchors() => new()
    {
        // payload, binary_score, regression_score, expected_label
        // banana
        { "ripe banana peel smoothie banana bunch banana bread", "0.95", 0.7, true },
        { "banana banana banana banana banana banana banana banana", "0.94", 0.8, true },
        // not banana
        { "plastic engine oil junk waste motor oil", "0.05", 0.1, false },
    };

    public static TheoryData<string, string, double, double, string> EscalatedAnchors() => new()
    {
        // payload, binary_score (in-band), transformer_score, regression, expected_label
        { "yellow plastic toy shaped like a banana", "0.50", 0.44, 0.4, "not_banana" },
        { "yellow fruit on the counter maybe", "0.55", 0.83, 0.6, "banana" },
        { "", "0.50", 0.65, 0.5, "banana" },
    };

    [Theory]
    [MemberData(nameof(CheapPathAnchors))]
    public async Task CheapPath_Anchors_ReturnNullEmbedding_AndDoNotCallTransformer(
        string payload, string binaryScore, double regression, bool expectedBanana)
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson = $"{{\"label\":\"{(expectedBanana ? "banana" : "not_banana")}\",\"banana_score\":{binaryScore}}}",
            // Set embedding-aware fake to OK to prove the cheap path does NOT call it.
            ClassifyTransformerWithEmbeddingStatus = NativeStatusCode.Ok,
            ClassifyTransformerWithEmbeddingJson = "{\"label\":\"banana\",\"banana_score\":0.99}",
            ClassifyTransformerWithEmbeddingValues = StubEmbedding,
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = regression,
        };

        var result = await RunEmbeddingRouteAsync(fake, payload);

        Assert.Null(result.Embedding);
        Assert.False(result.Verdict.DidEscalate);
        Assert.Equal(expectedBanana ? "banana" : "not_banana", result.Verdict.Label);
        Assert.Equal(0, fake.ClassifyTransformerWithEmbeddingCallCount);
    }

    [Theory]
    [MemberData(nameof(EscalatedAnchors))]
    public async Task EscalatedAnchors_ReturnFourDimEmbedding(
        string payload, string binaryScore, double transformerScore, double regression, string expectedLabel)
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson = $"{{\"label\":\"banana\",\"banana_score\":{binaryScore}}}",
            ClassifyTransformerWithEmbeddingStatus = NativeStatusCode.Ok,
            ClassifyTransformerWithEmbeddingJson =
                $"{{\"label\":\"{(transformerScore >= 0.5 ? "banana" : "not_banana")}\",\"banana_score\":{transformerScore.ToString("0.00", CultureInfo.InvariantCulture)}}}",
            ClassifyTransformerWithEmbeddingValues = StubEmbedding,
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = regression,
        };

        var result = await RunEmbeddingRouteAsync(fake, payload);

        Assert.NotNull(result.Embedding);
        Assert.Equal(4, result.Embedding!.Length);
        Assert.All(result.Embedding, d => Assert.True(double.IsFinite(d)));
        Assert.Equal(StubEmbedding, result.Embedding);
        Assert.True(result.Verdict.DidEscalate);
        Assert.Equal(expectedLabel, result.Verdict.Label);
        Assert.Equal(1, fake.ClassifyTransformerWithEmbeddingCallCount);
    }

    [Fact]
    public async Task EscalatedRouteUses_EmbeddingAware_NotLegacy_Transformer()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson = "{\"label\":\"banana\",\"banana_score\":0.50}",
            // Legacy transformer would be called by /ml/ensemble; on the
            // /ml/ensemble/embedding route only the embedding-aware variant
            // should be invoked.
            ClassifyBananaTransformerStatus = NativeStatusCode.InvalidArgument,
            ClassifyBananaTransformerJson = string.Empty,
            ClassifyTransformerWithEmbeddingStatus = NativeStatusCode.Ok,
            ClassifyTransformerWithEmbeddingJson = "{\"label\":\"banana\",\"banana_score\":0.83}",
            ClassifyTransformerWithEmbeddingValues = StubEmbedding,
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.5,
        };

        var result = await RunEmbeddingRouteAsync(fake, "ambiguous");

        Assert.True(result.Verdict.DidEscalate);
        Assert.NotNull(result.Embedding);
    }

    [Fact]
    public async Task LegacyEnsembleRoute_ShapeUnchanged_NoEmbeddingField()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson = "{\"label\":\"banana\",\"banana_score\":0.95}",
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.7,
        };

        var (controller, ctx) = MakeController(fake);
        ctx.Route = null;
        ctx.Ensemble = null;
        var result = await controller.Ensemble(new BananaMlController.MlRequest("{\"text\":\"banana\"}"), CancellationToken.None);
        var ok = Assert.IsType<OkObjectResult>(result);
        var verdict = Assert.IsType<EnsembleVerdictResult>(ok.Value);

        // Slice 014 contract -- 5 fields, no embedding.
        Assert.Equal("banana", verdict.Label);
        Assert.Equal("ok", verdict.Status);
        Assert.False(verdict.DidEscalate);
        // The legacy route's payload type is EnsembleVerdictResult, not the
        // 017 wrapper -- proves no field bleed.
        Assert.IsNotType<EnsembleVerdictWithEmbeddingResult>(ok.Value);
    }

    [Fact]
    public async Task EmbeddingRoute_BothBrainsFail_FloorsToUnknownDegraded_NoEmbedding()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.InvalidArgument,
            ClassifyBananaBinaryJson = string.Empty,
            ClassifyTransformerWithEmbeddingStatus = NativeStatusCode.InvalidArgument,
            ClassifyTransformerWithEmbeddingJson = string.Empty,
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.4,
        };

        var result = await RunEmbeddingRouteAsync(fake, "x");

        Assert.Equal("unknown", result.Verdict.Label);
        Assert.Equal("degraded", result.Verdict.Status);
        Assert.Null(result.Embedding); // P-R05 -- no embedding on degraded floor
    }

    private static async Task<EnsembleVerdictWithEmbeddingResult> RunEmbeddingRouteAsync(
        FakeNativeBananaClient fake, string payloadText)
    {
        var (controller, _) = MakeController(fake);
        var inputJson = $"{{\"text\":\"{payloadText.Replace("\"", "\\\"")}\"}}";
        var result = await controller.EnsembleWithEmbedding(
            new BananaMlController.MlRequest(inputJson), CancellationToken.None);
        var ok = Assert.IsType<OkObjectResult>(result);
        return Assert.IsType<EnsembleVerdictWithEmbeddingResult>(ok.Value);
    }

    private static (BananaMlController controller, PipelineContext ctx) MakeController(FakeNativeBananaClient fake)
    {
        var ctx = new PipelineContext();
        var mapper = new NoopMapper();
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new InputValidationStep(),
            new EnsembleGatingStep(fake),
            new EnsembleEscalationStep(fake),
            new EnsembleCalibrationStep(fake),
        };
        var runner = new PipelineRunner<PipelineContext>(steps);
        return (new BananaMlController(fake, mapper, ctx, runner), ctx);
    }

    private sealed class NoopMapper : INativeJsonMapper
    {
        public T? Deserialize<T>(string json) => default;
    }
}
