namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Managed representation of calculated points returned by the interop client.
/// </summary>
/// <param name="Purchases">Purchase count used in the calculation.</param>
/// <param name="Multiplier">Multiplier used in the calculation.</param>
/// <param name="Points">Final computed points total.</param>
/// <param name="Message">Human-readable message produced by native code.</param>
public sealed record PointsResult(int Purchases, int Multiplier, int Points, string Message);
