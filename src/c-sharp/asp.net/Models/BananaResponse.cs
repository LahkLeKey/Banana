namespace Banana.Api.Models;

/// <summary>
/// HTTP response contract returned by the <c>/banana</c> endpoint.
/// </summary>
/// <param name="Purchases">Purchase count used in the calculation.</param>
/// <param name="Multiplier">Multiplier used in the calculation.</param>
/// <param name="Banana">Final computed banana total.</param>
/// <param name="Message">Human-readable message describing the result.</param>
public sealed record BananaResponse(int Purchases, int Multiplier, int Banana, string Message);
