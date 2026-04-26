using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline.Results;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class NativeJsonMapperTests
{
    [Fact]
    public void Deserialize_BinaryClassificationResult_WithValidJson_ReturnsTypedResult()
    {
        var mapper = new NativeJsonMapper();
        const string json = "{\"label\":\"banana\",\"confidence\":0.88,\"model\":\"binary\",\"jaccard\":0.5}";

        var result = mapper.Deserialize<BinaryClassificationResult>(json);

        Assert.NotNull(result);
        Assert.Equal("banana", result.Label);
        Assert.Equal(0.88, result.Confidence, 3);
        Assert.Equal("binary", result.Model);
        Assert.True(result.Metrics.ContainsKey("jaccard"));
    }

    [Fact]
    public void Deserialize_NotBananaClassificationResult_WithValidJson_ReturnsTypedResult()
    {
        var mapper = new NativeJsonMapper();
        const string json = "{\"label\":\"not_banana\",\"model\":\"binary\",\"confusion_matrix\":{\"tp\":0,\"fp\":0,\"fn\":0,\"tn\":1}}";

        var result = mapper.Deserialize<NotBananaClassificationResult>(json);

        Assert.NotNull(result);
        Assert.Equal("not_banana", result.Label);
        Assert.Equal("binary", result.Model);
        Assert.True(result.Metrics.ContainsKey("confusion_matrix"));
    }

    [Fact]
    public void Deserialize_WithInvalidJson_ReturnsNull()
    {
        var mapper = new NativeJsonMapper();

        var result = mapper.Deserialize<BinaryClassificationResult>("{not-json");

        Assert.Null(result);
    }

    [Fact]
    public void Deserialize_WithWhitespace_ReturnsNull()
    {
        var mapper = new NativeJsonMapper();

        var result = mapper.Deserialize<BinaryClassificationResult>("   ");

        Assert.Null(result);
    }
}
