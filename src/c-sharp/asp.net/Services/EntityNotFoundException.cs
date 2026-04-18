namespace Banana.Api.Services;

/// <summary>
/// Represents a requested domain entity that could not be found.
/// </summary>
public sealed class EntityNotFoundException : Exception
{
    public EntityNotFoundException(string message) : base(message)
    {
    }
}