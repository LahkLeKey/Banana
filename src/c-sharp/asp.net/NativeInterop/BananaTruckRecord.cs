namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed representation of a tracked truck stored by native logistics logic.
/// </summary>
public sealed record BananaTruckRecord(
    string TruckId,
    string CurrentNodeId,
    string NodeType,
    double Latitude,
    double Longitude,
    double CapacityKg,
    double CurrentLoadKg,
    int ContainerCount);