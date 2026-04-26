using Banana.Api.NativeInterop;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Pipeline;

/// <summary>
/// Status code -> HTTP mapping (spec 007 contract).
/// </summary>
public static class StatusMapping
{
    public static IActionResult ToActionResult(NativeStatusCode code, object? body = null) => code switch
    {
        NativeStatusCode.Ok               => new OkObjectResult(body),
        NativeStatusCode.NotFound         => new NotFoundObjectResult(body ?? new { error = "not_found" }),
        NativeStatusCode.InvalidArgument  => new BadRequestObjectResult(body ?? new { error = "invalid_argument" }),
        NativeStatusCode.Overflow         => new BadRequestObjectResult(body ?? new { error = "overflow" }),
        NativeStatusCode.NativeUnavailable => new ObjectResult(body ?? new { error = "native_unavailable", remediation = "Set BANANA_NATIVE_PATH to a valid native library path." }) { StatusCode = 503 },
        NativeStatusCode.DbNotConfigured  => new ObjectResult(new { error = "config_missing", remediation = "Set BANANA_PG_CONNECTION." }) { StatusCode = 503 },
        NativeStatusCode.DbError          => new ObjectResult(new { error = "db_error", code = (int)code, remediation = "Check DAL dependency availability and PostgreSQL connectivity." }) { StatusCode = 500 },
        _                                 => new ObjectResult(new { error = "internal_error", code = (int)code }) { StatusCode = 500 },
    };
}
