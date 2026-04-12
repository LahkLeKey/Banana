namespace CInteropSharp.Api.DataAccess;

/// <summary>
/// Represents database-stage failures that should be surfaced consistently to API callers.
/// </summary>
public sealed class DatabaseAccessException : Exception
{
    /// <summary>
    /// Initializes a new instance of the <see cref="DatabaseAccessException"/> class.
    /// </summary>
    /// <param name="message">Safe error summary for logs and middleware mapping.</param>
    /// <param name="innerException">Underlying root cause.</param>
    public DatabaseAccessException(string message, Exception? innerException = null)
        : base(message, innerException)
    {
    }
}
