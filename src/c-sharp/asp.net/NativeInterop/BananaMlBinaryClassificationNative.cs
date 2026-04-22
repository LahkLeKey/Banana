using System.Runtime.InteropServices;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Native-compatible struct for banana binary classification output.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public struct BananaMlBinaryClassificationNative
{
    public BananaMlLabel PredictedLabel;
    public double BananaProbability;
    public double NotBananaProbability;
    public double DecisionMargin;
    public double JaccardSimilarity;
    public double ConfusionTruePositive;
    public double ConfusionFalsePositive;
    public double ConfusionFalseNegative;
    public double ConfusionTrueNegative;
}
