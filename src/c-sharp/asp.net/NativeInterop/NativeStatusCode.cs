namespace Banana.Api.NativeInterop;

/// <summary>
/// NativeStatusCode — numeric values mirror the native BananaStatusCode enum
/// (spec 006 contract). DO NOT renumber.
/// </summary>
public enum NativeStatusCode
{
    Ok = 0,
    InvalidArgument = 1,
    Overflow = 2,
    InternalError = 3,
    DbError = 4,
    DbNotConfigured = 5,
    NotFound = 6,
    BufferTooSmall = 7,
}
