namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Managed representation of calculated banana returned by the interop client.
/// </summary>
/// <param name="Purchases">Purchase count used in the calculation.</param>
/// <param name="Multiplier">Multiplier used in the calculation.</param>
/// <param name="Banana">Final computed banana total.</param>
/// <param name="Message">Human-readable message produced by native code.</param>
public sealed record BananaResult(int Purchases, int Multiplier, int Banana, string Message);
