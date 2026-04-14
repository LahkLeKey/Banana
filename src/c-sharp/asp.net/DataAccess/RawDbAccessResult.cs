namespace CInteropSharp.Api.DataAccess;

/// <summary>
/// Raw database output produced by a single data access stage execution.
/// </summary>
/// <param name="Source">Implementation source that produced this result.</param>
/// <param name="Payload">Serialized raw payload data.</param>
/// <param name="RowCount">Number of rows represented in the payload.</param>
public sealed record RawDbAccessResult(string Source, string Payload, int RowCount);
