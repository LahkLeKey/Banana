using System.Runtime.InteropServices;

namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Native-compatible struct for detailed points output.
/// Field order must remain aligned with the C wrapper definition.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public struct PointsBreakdownNative
{
    /// <summary>
    /// Purchase count echoed from native logic.
    /// </summary>
    public int Purchases;

    /// <summary>
    /// Multiplier echoed from native logic.
    /// </summary>
    public int Multiplier;

    /// <summary>
    /// Calculated points total from native logic.
    /// </summary>
    public int Points;
}
