using Banana.Api.Models;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("batches")]
/// <summary>
/// HTTP endpoints that manage tracked banana batches.
/// </summary>
public sealed class BatchController : ControllerBase
{
    private readonly IBatchService _batchService;

    public BatchController(IBatchService batchService)
    {
        _batchService = batchService;
    }

    [HttpPost("create")]
    [ProducesResponseType(typeof(BananaBatchResponse), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status500InternalServerError)]
    public ActionResult<BananaBatchResponse> Create([FromBody] BananaBatchCreateRequest request)
    {
        return Ok(_batchService.Create(request));
    }

    [HttpGet("{id}/status")]
    [ProducesResponseType(typeof(BananaBatchResponse), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public ActionResult<BananaBatchResponse> GetStatus([FromRoute] string id)
    {
        return Ok(_batchService.GetStatus(id));
    }
}