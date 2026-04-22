namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed soft confusion-matrix metrics produced by the binary banana classifier.
/// </summary>
public sealed record BananaMlBinaryConfusionMatrix(
    double TruePositive,
    double FalsePositive,
    double FalseNegative,
    double TrueNegative);
