namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Abstraction for managed-to-native points calculation calls.
/// </summary>
public interface INativePointsClient
{
    /// <summary>
    /// Calls native logic and returns managed-friendly results.
    /// </summary>
    /// <param name="purchases">Purchase count.</param>
    /// <param name="multiplier">Multiplier applied by native rules.</param>
    /// <returns>Calculated points and a human-readable message.</returns>
    PointsResult Calculate(int purchases, int multiplier);
}
