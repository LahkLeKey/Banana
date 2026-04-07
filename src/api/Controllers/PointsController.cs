using CInteropSharp.Api.Models;
using CInteropSharp.Api.Services;

using Microsoft.AspNetCore.Mvc;

namespace CInteropSharp.Api.Controllers;

[ApiController]
[Route("[controller]")]
public sealed class PointsController : ControllerBase
{
    private readonly IPointsService _pointsService;

    public PointsController(IPointsService pointsService)
    {
        _pointsService = pointsService;
    }

    [HttpGet("/points")]
    [ProducesResponseType(typeof(PointsResponse), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status500InternalServerError)]
    public ActionResult<PointsResponse> Get([FromQuery] int purchases, [FromQuery] int multiplier)
    {
        var result = _pointsService.Calculate(purchases, multiplier);

        return Ok(new PointsResponse(
            result.Purchases,
            result.Multiplier,
            result.Points,
            result.Message));
    }
}
