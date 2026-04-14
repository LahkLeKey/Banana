using System.Runtime.InteropServices;

namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Native-compatible struct for detailed banana output.
/// Field order must remain aligned with the C wrapper definition.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public struct BananaBreakdownNative
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
    /// Calculated banana total from native logic.
    /// </summary>
    public int Banana;
}
