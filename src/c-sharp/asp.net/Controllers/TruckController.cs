using System.Text.Json;
using System.Text.Json.Serialization;

using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;

using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>Banana truck registration and container lifecycle endpoints.</summary>
[ApiController]
[Route("trucks")]
public sealed class TruckController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    public sealed record InputJsonRequest(string InputJson);

    public sealed class TruckResult
    {
        [JsonPropertyName("truck_id")]
        public string TruckId { get; init; } = string.Empty;

        [JsonPropertyName("status")]
        public string Status { get; init; } = string.Empty;

        [JsonPropertyName("location")]
        public string Location { get; init; } = string.Empty;

        [JsonPropertyName("container_count")]
        public int ContainerCount { get; init; }
    }

    /// <summary>Registers a new truck and returns its ID and initial status.</summary>
    /// <param name="req">Truck registration JSON.</param>
    /// <response code="200"><see cref="TruckResult"/> with truck_id, status, location, and container_count.</response>
    [HttpPost("register")]
    public IActionResult Register([FromBody] InputJsonRequest req)
    {
        ctx.Route = "/trucks/register";
        var rc = native.RegisterTruck(req.InputJson, out var json);
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

    /// <summary>Loads a container onto a truck.</summary>
    /// <param name="truckId">Target truck identifier.</param>
    /// <param name="req">Container load JSON.</param>
    /// <response code="200">Updated <see cref="TruckResult"/>.</response>
    [HttpPost("{truckId}/containers/load")]
    public IActionResult Load([FromRoute] string truckId, [FromBody] InputJsonRequest req)
    {
        ctx.Route = $"/trucks/{truckId}/containers/load";
        var rc = native.LoadTruckContainer(truckId, req.InputJson, out var json);
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

    /// <summary>Unloads a specific container from a truck.</summary>
    /// <param name="truckId">Truck identifier.</param>
    /// <param name="containerId">Container identifier to unload.</param>
    /// <response code="200">Updated <see cref="TruckResult"/>.</response>
    [HttpPost("{truckId}/containers/{containerId}/unload")]
    public IActionResult Unload([FromRoute] string truckId, [FromRoute] string containerId)
    {
        ctx.Route = $"/trucks/{truckId}/containers/{containerId}/unload";
        var rc = native.UnloadTruckContainer(truckId, containerId, out var json);
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

    [HttpPost("{truckId}/relocate")]
    public IActionResult Relocate([FromRoute] string truckId, [FromBody] InputJsonRequest req)
    {
        ctx.Route = $"/trucks/{truckId}/relocate";
        var rc = native.RelocateTruck(truckId, req.InputJson, out var json);
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

    [HttpGet("{truckId}/status")]
    public IActionResult Status([FromRoute] string truckId)
    {
        ctx.Route = $"/trucks/{truckId}/status";
        var rc = native.GetTruckStatus(truckId, out var json);
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

    private static TruckResult? DeserializeResult(string json)
    {
        try
        {
            return JsonSerializer.Deserialize<TruckResult>(json, JsonOptions);
        }
        catch (JsonException)
        {
            return null;
        }
    }
}
