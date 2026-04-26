namespace Banana.Api.Pipeline.Mapping;

public interface INativeJsonMapper
{
    T? Deserialize<T>(string json);
}
