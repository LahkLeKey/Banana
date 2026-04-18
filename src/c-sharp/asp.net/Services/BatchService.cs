using Banana.Api.Models;
using Banana.Api.NativeInterop;

namespace Banana.Api.Services;

/// <summary>
/// Orchestrates tracked banana batch management through the native lifecycle registry.
/// </summary>
public sealed class BatchService : IBatchService
{
    private readonly INativeBananaClient _nativeBananaClient;

    public BatchService(INativeBananaClient nativeBananaClient)
    {
        _nativeBananaClient = nativeBananaClient;
    }

    public BananaBatchResponse Create(BananaBatchCreateRequest request)
    {
        if (request is null)
        {
            throw new ClientInputException("Batch request is required.");
        }

        if (string.IsNullOrWhiteSpace(request.BatchId) || string.IsNullOrWhiteSpace(request.OriginFarm))
        {
            throw new ClientInputException("batchId and originFarm are required.");
        }

        if (request.StorageTempC < 0.0 || request.EthyleneExposure < 0.0 || request.EstimatedShelfLifeDays < 0)
        {
            throw new ClientInputException("Batch telemetry values must be non-negative.");
        }

        var batch = _nativeBananaClient.CreateBatch(
            request.BatchId,
            request.OriginFarm,
            request.StorageTempC,
            request.EthyleneExposure,
            request.EstimatedShelfLifeDays);

        return new BananaBatchResponse(
            batch.BatchId,
            batch.OriginFarm,
            batch.ExportStatus,
            batch.StorageTempC,
            batch.EthyleneExposure,
            batch.EstimatedShelfLifeDays);
    }

    public BananaBatchResponse GetStatus(string batchId)
    {
        if (string.IsNullOrWhiteSpace(batchId))
        {
            throw new ClientInputException("batchId is required.");
        }

        var batch = _nativeBananaClient.GetBatchStatus(batchId);
        return new BananaBatchResponse(
            batch.BatchId,
            batch.OriginFarm,
            batch.ExportStatus,
            batch.StorageTempC,
            batch.EthyleneExposure,
            batch.EstimatedShelfLifeDays);
    }
}