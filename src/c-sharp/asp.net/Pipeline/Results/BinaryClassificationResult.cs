using System.Text.Json;
using System.Text.Json.Serialization;

namespace Banana.Api.Pipeline.Results;

public sealed class BinaryClassificationResult
{
    public string Label { get; init; } = string.Empty;
    public double Confidence { get; init; }
    public string Model { get; init; } = string.Empty;

    [JsonExtensionData]
    public Dictionary<string, JsonElement> Metrics { get; init; } = [];
}
