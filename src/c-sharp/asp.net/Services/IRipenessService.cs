using Banana.Api.Models;

namespace Banana.Api.Services;

/// <summary>
/// Application-level contract for predicting banana ripeness from telemetry.
/// </summary>
public interface IRipenessService
{
    BananaRipenessResponse Predict(BananaRipenessRequest request);
}