namespace Banana.Api.NativeInterop;

/// <summary>
/// Abstraction for managed-to-native banana calculation calls.
/// </summary>
public interface INativeBananaClient
{
    /// <summary>
    /// Calls native logic and returns managed-friendly results.
    /// </summary>
    /// <param name="purchases">Purchase count.</param>
    /// <param name="multiplier">Multiplier applied by native rules.</param>
    /// <returns>Calculated banana and a human-readable message.</returns>
    BananaResult Calculate(int purchases, int multiplier);
}
