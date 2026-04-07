using CInteropSharp.Api.NativeInterop;

namespace CInteropSharp.Api.Services;

public interface IPointsService
{
    PointsResult Calculate(int purchases, int multiplier);
}
