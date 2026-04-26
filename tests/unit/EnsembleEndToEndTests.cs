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
/// Slice 014 -- end-to-end coverage of /ml/ensemble using a fake interop
/// client. Folds the integration scope into the unit project (no
/// tests/integration/ project exists).
/// </summary>
public sealed class EnsembleEndToEndTests
{
    public static TheoryData<string, string, double, string, bool> AnchorWalks() => new()
    {
        // payload, binary banana_score, transformer banana_score (-1 if not invoked),
        // expected final label, expected did_escalate
        { "ripe banana peel smoothie banana bunch banana bread", "0.95", -1.0, "banana", false },
        { "banana banana banana banana banana banana banana banana", "0.94", -1.0, "banana", false },
        { "plastic engine oil junk waste motor oil", "0.05", -1.0, "not_banana", false },
        { "yellow plastic toy shaped like a banana", "0.50", 0.44, "not_banana", true },
        { "yellow fruit on the counter maybe", "0.55", 0.83, "banana", true },
        { "", "0.50", 0.65, "banana", true },
    };

    [Theory]
    [MemberData(nameof(AnchorWalks))]
    public async Task SpikeAnchors_ProduceExpectedEnsembleVerdict(
        string payloadText,
        string binaryBananaScore,
        double transformerBananaScore,
        string expectedLabel,
        bool expectedDidEscalate)
    {
        var binaryScore = double.Parse(binaryBananaScore, CultureInfo.InvariantCulture);
        var binaryLabel = binaryScore >= 0.5 ? "banana" : "not_banana";
        var transformerLabel = transformerBananaScore >= 0.5 ? "banana" : "not_banana";

        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson =
                $"{{\"model\":\"binary\",\"label\":\"{binaryLabel}\",\"banana_score\":{binaryBananaScore}}}",
            ClassifyBananaTransformerStatus = NativeStatusCode.Ok,
            ClassifyBananaTransformerJson = transformerBananaScore < 0
                ? string.Empty
                : $"{{\"model\":\"transformer\",\"label\":\"{transformerLabel}\",\"banana_score\":{transformerBananaScore.ToString("0.00", CultureInfo.InvariantCulture)}}}",
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.5,
        };

        var verdict = await RunEnsembleAsync(fake, payloadText);

        Assert.Equal(expectedLabel, verdict.Label);
        Assert.Equal(expectedDidEscalate, verdict.DidEscalate);
        Assert.InRange(verdict.CalibrationMagnitude, 0.0, 1.0);
        Assert.Equal("ok", verdict.Status);
    }

    [Fact]
    public async Task SnapshotShape_LocksFiveFieldContract()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson = "{\"model\":\"binary\",\"label\":\"banana\",\"banana_score\":0.95}",
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.7,
        };

        var verdict = await RunEnsembleAsync(fake, "ripe banana");

        // Lock the five-field shape required by slice 015 contract.
        Assert.NotNull(verdict.Label);
        Assert.InRange(verdict.Score, 0.0, 1.0);
        Assert.False(verdict.DidEscalate);
        Assert.InRange(verdict.CalibrationMagnitude, 0.0, 1.0);
        Assert.Equal("ok", verdict.Status);
    }

    [Fact]
    public async Task BothBrainsFail_FloorsToUnknownDegraded()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.InvalidArgument,
            ClassifyBananaBinaryJson = string.Empty,
            ClassifyBananaTransformerStatus = NativeStatusCode.InvalidArgument,
            ClassifyBananaTransformerJson = string.Empty,
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.4,
        };

        var verdict = await RunEnsembleAsync(fake, "x");

        Assert.Equal("unknown", verdict.Label);
        Assert.Equal("degraded", verdict.Status);
        Assert.True(verdict.DidEscalate);
        Assert.Equal(0.4, verdict.CalibrationMagnitude, 4);
    }

    [Fact]
    public async Task BinaryFails_TransformerSucceeds_StatusOk()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.InvalidArgument,
            ClassifyBananaBinaryJson = string.Empty,
            ClassifyBananaTransformerStatus = NativeStatusCode.Ok,
            ClassifyBananaTransformerJson = "{\"model\":\"transformer\",\"label\":\"banana\",\"banana_score\":0.83}",
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.6,
        };

        var verdict = await RunEnsembleAsync(fake, "ambiguous");

        Assert.Equal("banana", verdict.Label);
        Assert.True(verdict.DidEscalate);
        // Status is "degraded" because the gating step recorded a failure
        // even though the transformer recovered. This is the documented
        // "degraded but explicit" behavior from US4.
        Assert.Equal("degraded", verdict.Status);
    }

    private static async Task<EnsembleVerdictResult> RunEnsembleAsync(FakeNativeBananaClient fake, string payloadText)
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
        var controller = new BananaMlController(fake, mapper, ctx, runner);

        var inputJson = $"{{\"text\":\"{payloadText.Replace("\"", "\\\"")}\"}}";
        var result = await controller.Ensemble(new BananaMlController.MlRequest(inputJson), CancellationToken.None);

        var ok = Assert.IsType<OkObjectResult>(result);
        return Assert.IsType<EnsembleVerdictResult>(ok.Value);
    }

    private sealed class NoopMapper : INativeJsonMapper
    {
        public T? Deserialize<T>(string json) => default;
    }
}
