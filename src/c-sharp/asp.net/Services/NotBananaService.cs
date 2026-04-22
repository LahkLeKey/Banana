using System.Text.Json;
using System.Text.RegularExpressions;

using Banana.Api.Models;
using Banana.Api.NativeInterop;

namespace Banana.Api.Services;

/// <summary>
/// Orchestrates the polymorphic not-banana classifier through the native
/// interop client. Mirrors the Fastify gateway slice in
/// src/typescript/api/src/domains/not-banana/routes.ts so both runtimes agree
/// on normalisation, tokenisation, and classification labels.
/// </summary>
public sealed class NotBananaService : INotBananaService
{
    private const int MaxTokenisationDepth = 5;
    private const int MaxChildrenPerNode = 128;
    private const int MaxTokens = 4096;

    private static readonly string[] ActorTypeKeys = ["actorType", "type", "kind"];
    private static readonly string[] ActorIdKeys = ["actorId", "id", "actorKey"];
    private static readonly string[] EntityTypeKeys = ["entityType", "type", "kind"];
    private static readonly string[] EntityIdKeys = ["entityId", "id", "entityKey"];

    private static readonly Regex TokenRegex = new("[a-z0-9]+", RegexOptions.Compiled);

    private readonly INativeBananaClient _nativeBananaClient;

    public NotBananaService(INativeBananaClient nativeBananaClient)
    {
        _nativeBananaClient = nativeBananaClient;
    }

    public NotBananaJunkResponse Classify(NotBananaJunkRequest request)
    {
        if (request is null)
        {
            throw new ClientInputException("not-banana request is required.");
        }

        var actors = NormalizeActors(request);
        var entities = NormalizeEntities(request);

        var hasJunk = request.Junk is { ValueKind: not JsonValueKind.Undefined and not JsonValueKind.Null };
        var hasMetadata = request.Metadata is { ValueKind: not JsonValueKind.Undefined and not JsonValueKind.Null };

        if (actors.Count == 0 && entities.Count == 0 && !hasJunk && !hasMetadata)
        {
            throw new ClientInputException(
                "payload must include actors, entities, junk, or metadata content.");
        }

        var tokens = new HashSet<string>(StringComparer.Ordinal);

        foreach (var actor in actors)
        {
            AddStringTokens(actor.ActorType, tokens);
            AddStringTokens(actor.ActorId, tokens);
        }

        foreach (var entity in entities)
        {
            AddStringTokens(entity.EntityType, tokens);
            AddStringTokens(entity.EntityId, tokens);
        }

        if (request.Actors is { } actorPayloads)
        {
            foreach (var element in actorPayloads)
            {
                CollectTokens(element, tokens, depth: 0);
            }
        }

        if (request.Actor is { } singleActor)
        {
            CollectTokens(singleActor, tokens, depth: 0);
        }

        if (request.Entities is { } entityPayloads)
        {
            foreach (var element in entityPayloads)
            {
                CollectTokens(element, tokens, depth: 0);
            }
        }

        if (request.Entity is { } singleEntity)
        {
            CollectTokens(singleEntity, tokens, depth: 0);
        }

        if (hasJunk)
        {
            CollectTokens(request.Junk!.Value, tokens, depth: 0);
        }

        if (hasMetadata)
        {
            CollectTokens(request.Metadata!.Value, tokens, depth: 0);
        }

        var classification = _nativeBananaClient.ClassifyNotBananaJunk(
            tokens.ToArray(),
            actors.Count,
            entities.Count);

        var label = classification.PredictedLabel switch
        {
            "BANANA" => "MAYBE_BANANA",
            "INDETERMINATE" => "NOT_BANANA",
            _ => "NOT_BANANA"
        };

        var message = label == "NOT_BANANA"
            ? "object set classified as non-banana junk using polymorphic actor/entity analysis."
            : "object set contains banana-like signals; manual review is recommended.";

        return new NotBananaJunkResponse(
            label,
            RoundProbability(classification.BananaProbability),
            RoundProbability(classification.NotBananaProbability),
            RoundProbability(classification.JunkConfidence),
            classification.ActorCount,
            classification.EntityCount,
            actors,
            entities,
            message);
    }

    private static List<NotBananaPolymorphicActor> NormalizeActors(NotBananaJunkRequest request)
    {
        var collected = new List<NotBananaPolymorphicActor>();

        if (request.Actors is { } payloads)
        {
            foreach (var element in payloads)
            {
                collected.Add(NormalizeActor(element));
            }
        }

        if (request.Actor is { ValueKind: not JsonValueKind.Undefined and not JsonValueKind.Null } single)
        {
            collected.Add(NormalizeActor(single));
        }

        return collected;
    }

    private static List<NotBananaPolymorphicEntity> NormalizeEntities(NotBananaJunkRequest request)
    {
        var collected = new List<NotBananaPolymorphicEntity>();

        if (request.Entities is { } payloads)
        {
            foreach (var element in payloads)
            {
                collected.Add(NormalizeEntity(element));
            }
        }

        if (request.Entity is { ValueKind: not JsonValueKind.Undefined and not JsonValueKind.Null } single)
        {
            collected.Add(NormalizeEntity(single));
        }

        return collected;
    }

    private static NotBananaPolymorphicActor NormalizeActor(JsonElement element)
    {
        if (element.ValueKind != JsonValueKind.Object)
        {
            throw new ClientInputException("actor entries must be JSON objects.");
        }

        var actorType = ExtractStringField(element, ActorTypeKeys)
            ?? throw new ClientInputException("actor entries must include actorType/type/kind discriminator.");
        var actorId = ExtractStringField(element, ActorIdKeys);
        return new NotBananaPolymorphicActor(actorType, actorId);
    }

    private static NotBananaPolymorphicEntity NormalizeEntity(JsonElement element)
    {
        if (element.ValueKind != JsonValueKind.Object)
        {
            throw new ClientInputException("entity entries must be JSON objects.");
        }

        var entityType = ExtractStringField(element, EntityTypeKeys)
            ?? throw new ClientInputException("entity entries must include entityType/type/kind discriminator.");
        var entityId = ExtractStringField(element, EntityIdKeys);
        return new NotBananaPolymorphicEntity(entityType, entityId);
    }

    private static string? ExtractStringField(JsonElement element, IReadOnlyList<string> keys)
    {
        foreach (var key in keys)
        {
            if (element.TryGetProperty(key, out var value) && value.ValueKind == JsonValueKind.String)
            {
                var text = value.GetString();
                if (!string.IsNullOrWhiteSpace(text))
                {
                    return text.Trim();
                }
            }
        }

        return null;
    }

    private static void CollectTokens(JsonElement element, HashSet<string> tokens, int depth)
    {
        if (depth > MaxTokenisationDepth || tokens.Count >= MaxTokens)
        {
            return;
        }

        switch (element.ValueKind)
        {
            case JsonValueKind.String:
                AddStringTokens(element.GetString(), tokens);
                break;
            case JsonValueKind.Number:
                tokens.Add(element.GetRawText().ToLowerInvariant());
                break;
            case JsonValueKind.True:
                tokens.Add("true");
                break;
            case JsonValueKind.False:
                tokens.Add("false");
                break;
            case JsonValueKind.Array:
                {
                    var processed = 0;
                    foreach (var entry in element.EnumerateArray())
                    {
                        if (processed++ >= MaxChildrenPerNode)
                        {
                            break;
                        }

                        CollectTokens(entry, tokens, depth + 1);
                    }

                    break;
                }
            case JsonValueKind.Object:
                {
                    var processed = 0;
                    foreach (var property in element.EnumerateObject())
                    {
                        if (processed++ >= MaxChildrenPerNode)
                        {
                            break;
                        }

                        AddStringTokens(property.Name, tokens);
                        CollectTokens(property.Value, tokens, depth + 1);
                    }

                    break;
                }
        }
    }

    private static void AddStringTokens(string? value, HashSet<string> tokens)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return;
        }

        foreach (Match match in TokenRegex.Matches(value.ToLowerInvariant()))
        {
            if (tokens.Count >= MaxTokens)
            {
                return;
            }

            tokens.Add(match.Value);
        }
    }

    private static double RoundProbability(double value)
    {
        return Math.Round(value, 4, MidpointRounding.AwayFromZero);
    }
}
