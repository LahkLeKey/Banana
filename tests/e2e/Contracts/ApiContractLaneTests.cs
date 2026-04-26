using System.Reflection;
using System.Text.Json;

using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.E2eTests.Contracts;

public sealed class ApiContractLaneTests
{
    [Fact]
    public void RipenessRouteContractExposesPredictEndpoint()
    {
        var controllerRoute = GetControllerRouteTemplate<RipenessController>();
        var postTemplate = GetHttpPostTemplate<RipenessController>(nameof(RipenessController.Predict));

        Assert.Equal("ripeness", controllerRoute);
        Assert.Equal("predict", postTemplate);
    }

    [Fact]
    public void HarvestRouteContractExposesCreateAddBunchAndStatusEndpoints()
    {
        var controllerRoute = GetControllerRouteTemplate<HarvestController>();
        var createTemplate = GetHttpPostTemplate<HarvestController>(nameof(HarvestController.Create));
        var addBunchTemplate = GetHttpPostTemplate<HarvestController>(nameof(HarvestController.AddBunch));
        var statusTemplate = GetHttpGetTemplate<HarvestController>(nameof(HarvestController.Status));

        Assert.Equal("harvest", controllerRoute);
        Assert.Equal("create", createTemplate);
        Assert.Equal("{batchId}/bunches", addBunchTemplate);
        Assert.Equal("{batchId}/status", statusTemplate);
    }

    [Fact]
    public void TruckRouteContractExposesRegisterLoadUnloadRelocateAndStatusEndpoints()
    {
        var controllerRoute = GetControllerRouteTemplate<TruckController>();
        var registerTemplate = GetHttpPostTemplate<TruckController>(nameof(TruckController.Register));
        var loadTemplate = GetHttpPostTemplate<TruckController>(nameof(TruckController.Load));
        var unloadTemplate = GetHttpPostTemplate<TruckController>(nameof(TruckController.Unload));
        var relocateTemplate = GetHttpPostTemplate<TruckController>(nameof(TruckController.Relocate));
        var statusTemplate = GetHttpGetTemplate<TruckController>(nameof(TruckController.Status));

        Assert.Equal("trucks", controllerRoute);
        Assert.Equal("register", registerTemplate);
        Assert.Equal("{truckId}/containers/load", loadTemplate);
        Assert.Equal("{truckId}/containers/{containerId}/unload", unloadTemplate);
        Assert.Equal("{truckId}/relocate", relocateTemplate);
        Assert.Equal("{truckId}/status", statusTemplate);
    }

    [Fact]
    public void NativeUnavailableStatusContractMapsToDeterministic503Payload()
    {
        var result = StatusMapping.ToActionResult(NativeStatusCode.NativeUnavailable);

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(503, objectResult.StatusCode);

        var payload = SerializeToElement(objectResult.Value);
        Assert.Equal("native_unavailable", payload.GetProperty("error").GetString());
        Assert.Equal("Set BANANA_NATIVE_PATH to a valid native library path.", payload.GetProperty("remediation").GetString());
    }

    private static string GetControllerRouteTemplate<TController>()
        where TController : ControllerBase
    {
        var routeAttribute = typeof(TController).GetCustomAttribute<RouteAttribute>();
        Assert.NotNull(routeAttribute);
        return routeAttribute!.Template ?? string.Empty;
    }

    private static string GetHttpPostTemplate<TController>(string methodName)
        where TController : ControllerBase
    {
        var method = typeof(TController).GetMethod(methodName);
        Assert.NotNull(method);

        var postAttribute = method!.GetCustomAttribute<HttpPostAttribute>();
        Assert.NotNull(postAttribute);
        return postAttribute!.Template ?? string.Empty;
    }

    private static string GetHttpGetTemplate<TController>(string methodName)
        where TController : ControllerBase
    {
        var method = typeof(TController).GetMethod(methodName);
        Assert.NotNull(method);

        var getAttribute = method!.GetCustomAttribute<HttpGetAttribute>();
        Assert.NotNull(getAttribute);
        return getAttribute!.Template ?? string.Empty;
    }

    private static JsonElement SerializeToElement(object? value)
    {
        using var document = JsonDocument.Parse(JsonSerializer.Serialize(value));
        return document.RootElement.Clone();
    }
}
