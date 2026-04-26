using Banana.Api.NativeInterop;

namespace Banana.Api.Pipeline;

/// <summary>
/// Typed PipelineContext (spec 007). Adding a property is non-breaking;
/// removing one requires a step audit.
/// </summary>
public sealed class PipelineContext
{
    public string? Route { get; set; }
    public DateTime StartedAt { get; } = DateTime.UtcNow;
    public NativeStatusCode? LastStatus { get; set; }
    public Dictionary<string, string> Diagnostics { get; } = new(StringComparer.Ordinal);
}
