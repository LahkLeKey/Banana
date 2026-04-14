namespace Banana.Api.NativeInterop;

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
    InternalError = 3,

    /// <summary>
    /// Database query execution failed in native layer.
    /// </summary>
    DbError = 4,

    /// <summary>
    /// Native DB support is not configured.
    /// </summary>
    DbNotConfigured = 5
}
