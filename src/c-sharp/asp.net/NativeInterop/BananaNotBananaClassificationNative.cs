using System.Runtime.InteropServices;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Native-compatible struct for the polymorphic not-banana classifier output.
/// Layout must match CInteropBananaNotBananaClassification in
/// src/native/wrapper/banana_wrapper.h.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public struct BananaNotBananaClassificationNative
{
    public BananaNotBananaLabel PredictedLabel;
    public int ActorCount;
    public int EntityCount;
    public int SignalTokenCount;
    public int TotalTokenCount;
    public double BananaProbability;
    public double NotBananaProbability;
    public double JunkConfidence;
}
