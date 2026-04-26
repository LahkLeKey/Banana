using System.Text.Json;
using System.Text.Json.Serialization;

using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;

using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("harvest")]
public sealed class HarvestController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    public sealed record InputJsonRequest(string InputJson);

    public sealed class HarvestBatchResult
    {
        [JsonPropertyName("harvest_batch_id")]
        public string HarvestBatchId { get; init; } = string.Empty;

        [JsonPropertyName("status")]
        public string Status { get; init; } = string.Empty;

        [JsonPropertyName("bunch_count")]
        public int BunchCount { get; init; }
    }

    [HttpPost("create")]
    public IActionResult Create([FromBody] InputJsonRequest req)
    {
        ctx.Route = "/harvest/create";
        var rc = native.CreateHarvestBatch(req.InputJson, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = DeserializeResult(json);
        return result is null
            ? StatusCode(500, new { error = "invalid_native_payload" })
            : Ok(result);
    }

    [HttpPost("{batchId}/bunches")]
    public IActionResult AddBunch([FromRoute] string batchId, [FromBody] InputJsonRequest req)
    {
        ctx.Route = $"/harvest/{batchId}/bunches";
        var rc = native.AddBunchToHarvestBatch(batchId, req.InputJson, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = DeserializeResult(json);
        return result is null
            ? StatusCode(500, new { error = "invalid_native_payload" })
            : Ok(result);
    }

    [HttpGet("{batchId}/status")]
    public IActionResult Status([FromRoute] string batchId)
    {
        ctx.Route = $"/harvest/{batchId}/status";
        var rc = native.GetHarvestBatchStatus(batchId, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = DeserializeResult(json);
        return result is null
            ? StatusCode(500, new { error = "invalid_native_payload" })
            : Ok(result);
    }

    private static HarvestBatchResult? DeserializeResult(string json)
    {
        try
        {
            return JsonSerializer.Deserialize<HarvestBatchResult>(json, JsonOptions);
        }
        catch (JsonException)
        {
            return null;
        }
    }
}
