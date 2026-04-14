using CInteropSharp.Api.Models;
using CInteropSharp.Api.Services;

using Microsoft.AspNetCore.Mvc;

namespace CInteropSharp.Api.Controllers;

[ApiController]
[Route("[controller]")]
/// <summary>
/// HTTP endpoint that exposes banana calculation to external clients.
/// </summary>
public sealed class BananaController : ControllerBase
{
    private readonly IBananaService _bananaService;

    /// <summary>
    /// Initializes a new instance of the <see cref="BananaController"/> class.
    /// </summary>
    /// <param name="bananaService">Application service that executes the calculation pipeline.</param>
    public BananaController(IBananaService bananaService)
    {
        _bananaService = bananaService;
    }

    [HttpGet("/banana")]
    [ProducesResponseType(typeof(BananaResponse), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status500InternalServerError)]
    /// <summary>
    /// Calculates banana for the supplied query parameters.
    /// </summary>
    /// <param name="purchases">Purchase count supplied by the caller.</param>
    /// <param name="multiplier">Multiplier supplied by the caller.</param>
    /// <returns>Banana payload produced by service and native interop pipeline.</returns>
    public ActionResult<BananaResponse> Get([FromQuery] int purchases, [FromQuery] int multiplier)
    {
        var result = _bananaService.Calculate(purchases, multiplier);

        return Ok(new BananaResponse(
            result.Purchases,
            result.Multiplier,
            result.Banana,
            result.Message));
    }
}
