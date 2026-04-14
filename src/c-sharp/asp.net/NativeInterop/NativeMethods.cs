using System.Runtime.InteropServices;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Low-level P/Invoke signatures exposed by the C wrapper library.
/// </summary>
internal static partial class NativeMethods
{
    private const string LibraryName = "banana_native";

    /// <summary>
    /// Calls native calculation and returns banana only.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_calculate_banana")]
    internal static partial int CalculateBanana(int purchases, int multiplier, out int banana);

    /// <summary>
    /// Calls native calculation and returns a detailed breakdown struct.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_calculate_banana_with_breakdown")]
    internal static partial int CalculateBananaWithBreakdown(int purchases, int multiplier, out BananaBreakdownNative breakdown);

    /// <summary>
    /// Creates a native message string that must be released by <see cref="Free"/>.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_create_banana_message")]
    internal static partial int CreateBananaMessage(int purchases, int multiplier, out nint message);

    /// <summary>
    /// Executes native DB stage query and returns UTF-8 JSON payload allocated by native code.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_db_query_banana")]
    internal static partial int QueryDbBanana(int purchases, int multiplier, out nint payload, out int rowCount);

    /// <summary>
    /// Releases unmanaged memory allocated by the native wrapper.
    /// </summary>
    [LibraryImport(LibraryName, EntryPoint = "banana_free")]
    internal static partial void Free(nint pointer);
}
