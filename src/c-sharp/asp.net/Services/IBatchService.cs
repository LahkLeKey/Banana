using Banana.Api.Models;

namespace Banana.Api.Services;

/// <summary>
/// Application-level contract for managing tracked banana batches.
/// </summary>
public interface IBatchService
{
    BananaBatchResponse Create(BananaBatchCreateRequest request);

    BananaBatchResponse GetStatus(string batchId);
}