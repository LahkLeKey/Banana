using System.Runtime.InteropServices;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Native-compatible struct for banana transformer classification output.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public struct BananaMlTransformerClassificationNative
{
    public BananaMlLabel PredictedLabel;
    public double BananaProbability;
    public double NotBananaProbability;
    public double AttentionFocus;
}
