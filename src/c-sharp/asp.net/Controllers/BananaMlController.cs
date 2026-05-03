using Banana.Api.NativeInterop;
using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline.Results;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>Machine-learning classification and ensemble inference endpoints.</summary>
[ApiController]
[Route("ml")]
public sealed class BananaMlController(
    INativeBananaClient native,
    INativeJsonMapper mapper,
    PipelineContext ctx,
    PipelineRunner<PipelineContext> runner) : ControllerBase
{
    /// <summary>Request body carrying raw feature JSON produced by the native pipeline.</summary>
    public sealed record MlRequest(string InputJson);

    /// <summary>Runs the native regression model and returns a continuous banana score.</summary>
    /// <param name="req">Feature input JSON.</param>
    /// <response code="200">JSON with <c>score</c> (double).</response>
    /// <response code="400">Input validation failed.</response>
    [HttpPost("regression")]
    public IActionResult Regression([FromBody] MlRequest req)
    {
        ctx.Route = "/ml/regression";
        var rc = native.PredictRegressionScore(req.InputJson, out var score);
        ctx.LastStatus = rc;
        return rc == NativeStatusCode.Ok
            ? Ok(new { score })
            : StatusMapping.ToActionResult(rc);
    }

    /// <summary>Runs the native binary classifier and returns a labelled banana/not-banana result.</summary>
    /// <param name="req">Feature input JSON.</param>
    /// <response code="200"><see cref="BinaryClassificationResult"/> with label and score.</response>
    [HttpPost("binary")]
    public IActionResult Binary([FromBody] MlRequest req)
    {
        ctx.Route = "/ml/binary";
        var rc = native.ClassifyBananaBinary(req.InputJson, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = mapper.Deserialize<BinaryClassificationResult>(json);
        if (result is null)
        {
            return StatusCode(500, new { error = "invalid_native_payload" });
        }

        return Ok(result);
    }

    /// <summary>Runs the Full Brain transformer model and returns a 4-class banana classification.</summary>
    /// <param name="req">Feature input JSON.</param>
    /// <response code="200"><see cref="TransformerClassificationResult"/> with label, score, and embedding.</response>
    [HttpPost("transformer")]
    public IActionResult Transformer([FromBody] MlRequest req)
    {
        ctx.Route = "/ml/transformer";
        var rc = native.ClassifyBananaTransformer(req.InputJson, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = mapper.Deserialize<TransformerClassificationResult>(json);
        if (result is null)
        {
            return StatusCode(500, new { error = "invalid_native_payload" });
        }

        return Ok(result);
    }

    /// <summary>
    /// Slice 014 -- gated cascade ensemble route. Runs the full pipeline
    /// (input validation + ensemble gating + escalation + calibration) and
    /// returns a single composite EnsembleVerdictResult. Cascade band is
    /// baked at [0.35, 0.65] on the binary banana_score.
    /// </summary>
    [HttpPost("ensemble")]
    public async Task<IActionResult> Ensemble([FromBody] MlRequest req, CancellationToken ct)
    {
        ctx.Route = "/ml/ensemble";
        ctx.InputJson = req.InputJson;
        ctx.Ensemble = new EnsembleWorkingVerdict();

        var pipelineResult = await runner.RunAsync(ctx, ct);
        if (!pipelineResult.IsSuccess)
        {
            return BadRequest(pipelineResult.Problem);
        }

        var working = ctx.Ensemble;
        var verdict = new EnsembleVerdictResult(
            Label: working.Label,
            Score: working.Score,
            DidEscalate: working.DidEscalate,
            CalibrationMagnitude: working.CalibrationMagnitude,
            Status: working.Degraded ? "degraded" : "ok");

        return Ok(verdict);
    }

    /// <summary>
    /// Slice 017 -- additive variant of <c>/ml/ensemble</c> that also returns
    /// the Full Brain transformer's 4-dim embedding fingerprint when the
    /// cascade escalated. Cheap-path verdicts return embedding=null. The
    /// legacy <c>/ml/ensemble</c> JSON shape is unchanged.
    /// </summary>
    [HttpPost("ensemble/embedding")]
    public async Task<IActionResult> EnsembleWithEmbedding([FromBody] MlRequest req, CancellationToken ct)
    {
        ctx.Route = "/ml/ensemble/embedding";
        ctx.InputJson = req.InputJson;
        ctx.Ensemble = new EnsembleWorkingVerdict();

        var pipelineResult = await runner.RunAsync(ctx, ct);
        if (!pipelineResult.IsSuccess)
        {
            return BadRequest(pipelineResult.Problem);
        }

        var working = ctx.Ensemble;
        var verdict = new EnsembleVerdictResult(
            Label: working.Label,
            Score: working.Score,
            DidEscalate: working.DidEscalate,
            CalibrationMagnitude: working.CalibrationMagnitude,
            Status: working.Degraded ? "degraded" : "ok");

        // Embedding is captured by the escalation step iff the cascade
        // escalated AND the route is /ml/ensemble/embedding (P-R05).
        var embedding = working.DidEscalate && !working.Degraded ? working.CapturedEmbedding : null;
        return Ok(new EnsembleVerdictWithEmbeddingResult(verdict, embedding));
    }
}
