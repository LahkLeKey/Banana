namespace Banana.Api.NativeInterop;

/// <summary>
/// Ripeness stages returned by native lifecycle logic.
/// </summary>
public enum BananaRipenessStage
{
    Green = 0,
    Breaking = 1,
    Yellow = 2,
    Spotted = 3,
    Overripe = 4,
    Biodegradation = 5
}