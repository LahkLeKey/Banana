namespace CInteropSharp.Api.Models;

/// <summary>
/// HTTP response contract returned by the <c>/points</c> endpoint.
/// </summary>
/// <param name="Purchases">Purchase count used in the calculation.</param>
/// <param name="Multiplier">Multiplier used in the calculation.</param>
/// <param name="Points">Final computed points total.</param>
/// <param name="Message">Human-readable message describing the result.</param>
public sealed record PointsResponse(int Purchases, int Multiplier, int Points, string Message);
