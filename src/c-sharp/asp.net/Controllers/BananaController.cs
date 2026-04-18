using Banana.Api.Models;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("[controller]")]
/// <summary>
/// HTTP endpoint that exposes banana calculation to external clients.
/// </summary>
public sealed class BananaController : ControllerBase
{
    private const string DbContractHeader = "X-Banana-Db-Contract";
    private const string DbSourceHeader = "X-Banana-Db-Source";
    private const string DbRowCountHeader = "X-Banana-Db-RowCount";

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

        WriteMetadataHeader(DbContractHeader, result.Metadata, "db.contract");
        WriteMetadataHeader(DbSourceHeader, result.Metadata, "db.source");
        WriteMetadataHeader(DbRowCountHeader, result.Metadata, "db.rowCount");

        return Ok(new BananaResponse(
            result.BananaResult.Purchases,
            result.BananaResult.Multiplier,
            result.BananaResult.Banana,
            result.BananaResult.Message));
    }

    private void WriteMetadataHeader(string headerName, IReadOnlyDictionary<string, object> metadata, string metadataKey)
    {
        if (!metadata.TryGetValue(metadataKey, out var value) || value is null)
        {
            return;
        }

        Response.Headers[headerName] = value.ToString();
    }
}
