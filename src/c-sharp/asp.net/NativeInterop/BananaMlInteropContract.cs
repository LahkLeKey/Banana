namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed constants that mirror the native wrapper ABI contract for banana ML model inputs.
/// </summary>
internal static class BananaMlInteropContract
{
    public const int FeatureCount = 8;
    public const int TokenFeatureCount = 4;
    public const int MaxSequenceLength = 16;
}
