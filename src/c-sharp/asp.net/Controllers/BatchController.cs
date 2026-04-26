using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("batches")]
public sealed class BatchController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    public sealed record CreateBatchRequest(string InputJson);

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
