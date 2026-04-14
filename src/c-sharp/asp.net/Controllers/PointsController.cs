using CInteropSharp.Api.Models;
using CInteropSharp.Api.Services;

using Microsoft.AspNetCore.Mvc;

namespace CInteropSharp.Api.Controllers;

[ApiController]
[Route("[controller]")]
/// <summary>
/// HTTP endpoint that exposes points calculation to external clients.
/// </summary>
public sealed class PointsController : ControllerBase
{
    private readonly IPointsService _pointsService;

    /// <summary>
    /// Initializes a new instance of the <see cref="PointsController"/> class.
    /// </summary>
    /// <param name="pointsService">Application service that executes the calculation pipeline.</param>
    public PointsController(IPointsService pointsService)
    {
        _pointsService = pointsService;
    }

    [HttpGet("/points")]
    [ProducesResponseType(typeof(PointsResponse), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status500InternalServerError)]
    /// <summary>
    /// Calculates points for the supplied query parameters.
    /// </summary>
    /// <param name="purchases">Purchase count supplied by the caller.</param>
    /// <param name="multiplier">Multiplier supplied by the caller.</param>
    /// <returns>Points payload produced by service and native interop pipeline.</returns>
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
