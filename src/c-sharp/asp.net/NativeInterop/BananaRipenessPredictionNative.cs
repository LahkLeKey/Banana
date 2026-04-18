using System.Runtime.InteropServices;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Native-compatible struct for ripeness prediction output.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public struct BananaRipenessPredictionNative
{
    public int PredictedStage;
    public int ShelfLifeHours;
    public double RipeningIndex;
    public double SpoilageProbability;
    public double ColdChainRisk;
}