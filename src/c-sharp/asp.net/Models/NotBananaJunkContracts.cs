namespace Banana.Api.Models;

/// <summary>
/// Polymorphic actor record carried by a not-banana junk request.
/// Mirrors the Fastify gateway contract in
/// src/typescript/api/src/contracts/http.ts.
/// </summary>
public sealed record NotBananaPolymorphicActor(
    string ActorType,
    string? ActorId);

/// <summary>
/// Polymorphic entity record carried by a not-banana junk request.
/// </summary>
public sealed record NotBananaPolymorphicEntity(
    string EntityType,
    string? EntityId);

/// <summary>
/// Polymorphic junk classification request. All fields are optional but at
/// least one of <see cref="Actors"/>, <see cref="Entities"/>,
/// <see cref="Actor"/>, <see cref="Entity"/>, or <see cref="Junk"/> must
/// supply a value. Arbitrary metadata and free-form junk content are
/// preserved as <see cref="System.Text.Json.JsonElement"/> trees so the
/// classifier can tokenise them without losing structure.
/// </summary>
public sealed class NotBananaJunkRequest
{
    public List<System.Text.Json.JsonElement>? Actors { get; init; }

    public List<System.Text.Json.JsonElement>? Entities { get; init; }

    public System.Text.Json.JsonElement? Actor { get; init; }

    public System.Text.Json.JsonElement? Entity { get; init; }

    public System.Text.Json.JsonElement? Junk { get; init; }

    public System.Text.Json.JsonElement? Metadata { get; init; }
}

/// <summary>
/// Polymorphic junk classification response.
/// </summary>
public sealed record NotBananaJunkResponse(
    string Classification,
    double BananaProbability,
    double NotBananaProbability,
    double JunkConfidence,
    int ActorCount,
    int EntityCount,
    IReadOnlyList<NotBananaPolymorphicActor> NormalizedActors,
    IReadOnlyList<NotBananaPolymorphicEntity> NormalizedEntities,
    string Message);
