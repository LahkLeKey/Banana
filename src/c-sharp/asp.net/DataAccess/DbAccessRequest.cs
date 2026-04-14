namespace Banana.Api.DataAccess;

/// <summary>
/// Input envelope passed into the atomic database stage execution.
/// </summary>
/// <param name="Purchases">Purchase count from request context.</param>
/// <param name="Multiplier">Multiplier from request context.</param>
public sealed record DbAccessRequest(int Purchases, int Multiplier);
