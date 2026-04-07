using System.Runtime.InteropServices;

namespace CInteropSharp.Api.NativeInterop;

[StructLayout(LayoutKind.Sequential)]
public struct PointsBreakdownNative
{
    public int Purchases;
    public int Multiplier;
    public int Points;
}
