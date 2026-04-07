namespace CInteropSharp.Api.NativeInterop;

public interface INativePointsClient
{
    PointsResult Calculate(int purchases, int multiplier);
}
