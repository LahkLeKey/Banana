using Banana.Api.NativeInterop;

namespace Banana.Api.Services;

/// <summary>
/// Application-level contract for calculating banana for API consumers.
/// </summary>
public interface IBananaService
{
    /// <summary>
    /// Calculates banana for the given purchase inputs.
    /// </summary>
    /// <param name="purchases">Number of purchases.</param>
    /// <param name="multiplier">Business multiplier applied by native logic.</param>
    /// <returns>Calculated banana and a message.</returns>
    BananaResult Calculate(int purchases, int multiplier);
}
