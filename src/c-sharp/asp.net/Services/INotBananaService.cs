using Banana.Api.Models;

namespace Banana.Api.Services;

/// <summary>
/// Application-level contract for the polymorphic not-banana classifier.
/// </summary>
public interface INotBananaService
{
    NotBananaJunkResponse Classify(NotBananaJunkRequest request);
}
