using CInteropSharp.Api.NativeInterop;

namespace CInteropSharp.Api.Services;

public sealed class PointsService : IPointsService
{
    private readonly INativePointsClient _nativePointsClient;

    public PointsService(INativePointsClient nativePointsClient)
    {
        _nativePointsClient = nativePointsClient;
    }

    public PointsResult Calculate(int purchases, int multiplier)
    {
        if (purchases < 0 || multiplier < 0)
        {
            throw new ClientInputException("purchases and multiplier must be non-negative.");
        }

        return _nativePointsClient.Calculate(purchases, multiplier);
    }
}

public sealed class ClientInputException : Exception
{
    public ClientInputException(string message) : base(message)
    {
    }
}
