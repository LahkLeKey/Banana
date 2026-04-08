using System.Runtime.InteropServices;

namespace CInteropSharp.Api.NativeInterop;

/// <summary>
/// Low-level P/Invoke entry points exposed by the C wrapper library.
/// </summary>
internal static partial class NativeMethods
{
    private const string LibraryName = "cinterop_native";

    /// <summary>
    /// Calls native calculation and returns points only.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "cinterop_calculate_points")]
    internal static partial int CalculatePoints(int purchases, int multiplier, out int points);

    /// <summary>
    /// Calls native calculation and returns a detailed breakdown struct.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "cinterop_calculate_points_with_breakdown")]
    internal static partial int CalculatePointsWithBreakdown(int purchases, int multiplier, out PointsBreakdownNative breakdown);

    /// <summary>
    /// Creates a native message string that must be released by <see cref="Free"/>.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "cinterop_create_points_message")]
    internal static partial int CreatePointsMessage(int purchases, int multiplier, out nint message);

    /// <summary>
    /// Releases unmanaged memory allocated by the native wrapper.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "cinterop_free")]
    internal static partial void Free(nint pointer);
}
