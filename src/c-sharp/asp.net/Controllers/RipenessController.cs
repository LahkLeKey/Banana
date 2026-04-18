using Banana.Api.Models;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("ripeness")]
/// <summary>
/// HTTP endpoint that exposes banana ripeness prediction to external clients.
/// </summary>
public sealed class RipenessController : ControllerBase
{
    private readonly IRipenessService _ripenessService;

    public RipenessController(IRipenessService ripenessService)
    {
        _ripenessService = ripenessService;
    }

    [HttpPost("predict")]
    [ProducesResponseType(typeof(BananaRipenessResponse), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status500InternalServerError)]
    /// <summary>
    /// Predicts ripeness for a banana batch using telemetry history.
    /// </summary>
    public ActionResult<BananaRipenessResponse> Predict([FromBody] BananaRipenessRequest request)
    {
        return Ok(_ripenessService.Predict(request));
    }
}