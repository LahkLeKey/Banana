using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>Banana batch lifecycle endpoints — create and poll status.</summary>
[ApiController]
[Route("batches")]
public sealed class BatchController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    /// <summary>Request body for creating a new batch.</summary>
    public sealed record CreateBatchRequest(string InputJson);

    /// <summary>Creates a new banana batch and returns its identifier.</summary>
    /// <param name="req">Batch input JSON.</param>
    /// <response code="200">JSON with batch ID and initial metadata.</response>
    [HttpPost("create")]
    public IActionResult Create([FromBody] CreateBatchRequest req)
    {
        ctx.Route = "/batches/create";
        var rc = native.CreateBatch(req.InputJson, out var json);
        ctx.LastStatus = rc;
        return rc == NativeStatusCode.Ok
            ? Ok(new { json })
            : StatusMapping.ToActionResult(rc);
    }

    /// <summary>Returns the current status of a batch by ID.</summary>
    /// <param name="id">Batch identifier returned by <c>POST /batches/create</c>.</param>
    /// <response code="200">Batch status JSON.</response>
    /// <response code="404">Batch not found.</response>
    [HttpGet("{id}/status")]
    public IActionResult Status([FromRoute] string id)
    {
        ctx.Route = $"/batches/{id}/status";
        var rc = native.GetBatchStatus(id, out var json);
        ctx.LastStatus = rc;
        return rc == NativeStatusCode.Ok
            ? Ok(new { id, json })
            : StatusMapping.ToActionResult(rc);
    }
}
