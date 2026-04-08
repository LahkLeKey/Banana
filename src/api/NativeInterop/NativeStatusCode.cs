namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Status codes returned by the native wrapper ABI.
/// </summary>
public enum NativeStatusCode
{
    /// <summary>
    /// Operation completed successfully.
    /// </summary>
    Ok = 0,

    /// <summary>
    /// Input arguments were invalid.
    /// </summary>
    InvalidArgument = 1,

    /// <summary>
    /// Input values overflowed native constraints.
    /// </summary>
    Overflow = 2,

    /// <summary>
    /// Native code failed unexpectedly.
    /// </summary>
    InternalError = 3
}
