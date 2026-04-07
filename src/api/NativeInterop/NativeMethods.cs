using System.Runtime.InteropServices;

namespace CInteropSharp.Api.NativeInterop;

internal static partial class NativeMethods
{
    private const string LibraryName = "cinterop_native";

    [LibraryImport(LibraryName, EntryPoint = "cinterop_calculate_points")]
    internal static partial int CalculatePoints(int purchases, int multiplier, out int points);

    [LibraryImport(LibraryName, EntryPoint = "cinterop_calculate_points_with_breakdown")]
    internal static partial int CalculatePointsWithBreakdown(int purchases, int multiplier, out PointsBreakdownNative breakdown);

    [LibraryImport(LibraryName, EntryPoint = "cinterop_create_points_message")]
    internal static partial int CreatePointsMessage(int purchases, int multiplier, out nint message);

    [LibraryImport(LibraryName, EntryPoint = "cinterop_free")]
    internal static partial void Free(nint pointer);
}
