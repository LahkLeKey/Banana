using Banana.Api.Models;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("not-banana")]
/// <summary>
/// HTTP endpoint that exposes the polymorphic not-banana classifier to
/// external clients. Mirrors the Fastify gateway route at
/// src/typescript/api/src/domains/not-banana/routes.ts.
/// </summary>
public sealed class NotBananaController : ControllerBase
{
    private readonly INotBananaService _notBananaService;

    public NotBananaController(INotBananaService notBananaService)
    {
        _notBananaService = notBananaService;
    }

    [HttpPost("junk")]
    [ProducesResponseType(typeof(NotBananaJunkResponse), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status500InternalServerError)]
    public ActionResult<NotBananaJunkResponse> Classify([FromBody] NotBananaJunkRequest request)
    {
        return Ok(_notBananaService.Classify(request));
    }
}
