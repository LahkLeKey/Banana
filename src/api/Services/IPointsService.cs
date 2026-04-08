using CInteropSharp.Api.NativeInterop;

namespace CInteropSharp.Api.Services;

/// <summary>
/// Application-level contract for calculating points for API consumers.
/// </summary>
public interface IPointsService
{
    /// <summary>
    /// Calculates points for the given purchase inputs.
    /// </summary>
    /// <param name="purchases">Number of purchases.</param>
    /// <param name="multiplier">Business multiplier applied by native logic.</param>
    /// <returns>Calculated points and a message.</returns>
    PointsResult Calculate(int purchases, int multiplier);
}
