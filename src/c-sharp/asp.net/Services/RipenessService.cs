using Banana.Api.Models;
using Banana.Api.NativeInterop;

namespace Banana.Api.Services;

/// <summary>
/// Orchestrates banana ripeness prediction through the native interop client.
/// </summary>
public sealed class RipenessService : IRipenessService
{
    private readonly INativeBananaClient _nativeBananaClient;

    public RipenessService(INativeBananaClient nativeBananaClient)
    {
        _nativeBananaClient = nativeBananaClient;
    }

    public BananaRipenessResponse Predict(BananaRipenessRequest request)
    {
        if (request is null)
        {
            throw new ClientInputException("Ripeness request is required.");
        }

        if (string.IsNullOrWhiteSpace(request.BatchId))
        {
            throw new ClientInputException("batchId is required.");
        }

        if (request.TemperatureHistory is null || request.TemperatureHistory.Count == 0)
        {
            throw new ClientInputException("temperatureHistory must contain at least one reading.");
        }

        if (request.DaysSinceHarvest < 0)
        {
            throw new ClientInputException("daysSinceHarvest must be non-negative.");
        }

        if (request.EthyleneExposure < 0.0 || request.MechanicalDamage < 0.0)
        {
            throw new ClientInputException("ethyleneExposure and mechanicalDamage must be non-negative.");
        }

        var prediction = _nativeBananaClient.PredictBatchRipeness(
            request.BatchId,
            request.TemperatureHistory,
            request.DaysSinceHarvest,
            request.MechanicalDamage);

        return new BananaRipenessResponse(
            request.BatchId,
            prediction.PredictedStage,
            prediction.ShelfLifeHours,
            prediction.RipeningIndex,
            prediction.SpoilageProbability,
            prediction.ColdChainRisk);
    }
}