using System.Runtime.InteropServices;

namespace Banana.Api.NativeInterop;

/// <summary>
/// Concrete implementation of the interop seam. All native call sites live
/// here exactly once (spec 007). Callee-allocated strings are freed via
/// <see cref="NativeMethods.Free"/>.
/// </summary>
public sealed class NativeBananaClient : INativeBananaClient
{
    public NativeStatusCode CalculateBanana(int purchases, int multiplier, out int total)
        => (NativeStatusCode)NativeMethods.CalculateBanana(purchases, multiplier, out total);

    public NativeStatusCode CalculateBananaWithBreakdown(int purchases, int multiplier, out int total, out int baseAmount, out int bonus)
        => (NativeStatusCode)NativeMethods.CalculateBananaWithBreakdown(purchases, multiplier, out total, out baseAmount, out bonus);

    public NativeStatusCode CreateBananaMessage(int purchases, int multiplier, out string message)
    {
        var rc = NativeMethods.CreateBananaMessage(purchases, multiplier, out var ptr);
        message = TakeUtf8(ptr);
        return (NativeStatusCode)rc;
    }

    public NativeStatusCode QueryBananaProfile(string profileId, out string json)
    {
        var rc = NativeMethods.QueryBananaProfile(profileId, out var ptr);
        json = TakeUtf8(ptr);
        return (NativeStatusCode)rc;
    }

    public NativeStatusCode PredictRegressionScore(string inputJson, out double score)
    {
        try
        {
            return (NativeStatusCode)NativeMethods.PredictRegressionScore(inputJson, out score);
        }
        catch (Exception ex) when (IsInteropUnavailable(ex))
        {
            score = 0.0;
            return NativeStatusCode.NativeUnavailable;
        }
    }

    public NativeStatusCode ClassifyBananaBinary(string inputJson, out string json) => CallJson(inputJson, out json, NativeMethods.ClassifyBananaBinary);
    public NativeStatusCode ClassifyBananaTransformer(string inputJson, out string json) => CallJson(inputJson, out json, NativeMethods.ClassifyBananaTransformer);

    public NativeStatusCode ClassifyBananaTransformerWithEmbedding(string inputJson, double[] embedding, out string json)
    {
        ArgumentNullException.ThrowIfNull(embedding);
        if (embedding.Length != 4) throw new ArgumentException("embedding must be exactly 4 doubles (BANANA_ML_TRANSFORMER_EMBEDDING_DIM)", nameof(embedding));
        try
        {
            var rc = NativeMethods.ClassifyBananaTransformerEx(
                inputJson,
                logAttention: 0,
                outEmbedding: ref embedding[0],
                outAttentionWeights: IntPtr.Zero,
                out var ptr);
            json = TakeUtf8(ptr);
            return (NativeStatusCode)rc;
        }
        catch (Exception ex) when (IsInteropUnavailable(ex))
        {
            json = string.Empty;
            return NativeStatusCode.NativeUnavailable;
        }
    }
    public NativeStatusCode ClassifyNotBananaJunk(string inputJson, out string json) => CallJson(inputJson, out json, NativeMethods.ClassifyNotBananaJunk);
    public NativeStatusCode PredictBananaRipeness(string inputJson, out string json) => CallJson(inputJson, out json, NativeMethods.PredictBananaRipeness);
    public NativeStatusCode CreateBatch(string inputJson, out string json) => CallJson(inputJson, out json, NativeMethods.CreateBatch);
    public NativeStatusCode GetBatchStatus(string batchId, out string json) => CallJson(batchId, out json, NativeMethods.GetBatchStatus);
    public NativeStatusCode PredictBatchRipeness(string batchId, out string json) => CallJson(batchId, out json, NativeMethods.PredictBatchRipeness);
    public NativeStatusCode CreateHarvestBatch(string inputJson, out string json) => CallJson(inputJson, out json, NativeMethods.CreateHarvestBatch);
    public NativeStatusCode GetHarvestBatchStatus(string batchId, out string json) => CallJson(batchId, out json, NativeMethods.GetHarvestBatchStatus);
    public NativeStatusCode RegisterTruck(string inputJson, out string json) => CallJson(inputJson, out json, NativeMethods.RegisterTruck);
    public NativeStatusCode GetTruckStatus(string truckId, out string json) => CallJson(truckId, out json, NativeMethods.GetTruckStatus);

    public NativeStatusCode AddBunchToHarvestBatch(string batchId, string inputJson, out string json)
    {
        var rc = NativeMethods.AddBunchToHarvestBatch(batchId, inputJson, out var ptr);
        json = TakeUtf8(ptr);
        return (NativeStatusCode)rc;
    }

    public NativeStatusCode LoadTruckContainer(string truckId, string inputJson, out string json)
    {
        var rc = NativeMethods.LoadTruckContainer(truckId, inputJson, out var ptr);
        json = TakeUtf8(ptr);
        return (NativeStatusCode)rc;
    }

    public NativeStatusCode UnloadTruckContainer(string truckId, string containerId, out string json)
    {
        var rc = NativeMethods.UnloadTruckContainer(truckId, containerId, out var ptr);
        json = TakeUtf8(ptr);
        return (NativeStatusCode)rc;
    }

    public NativeStatusCode RelocateTruck(string truckId, string inputJson, out string json)
    {
        var rc = NativeMethods.RelocateTruck(truckId, inputJson, out var ptr);
        json = TakeUtf8(ptr);
        return (NativeStatusCode)rc;
    }

    private delegate int Single(string s, out IntPtr ptr);
    private static NativeStatusCode CallJson(string s, out string json, Single fn)
    {
        try
        {
            var rc = fn(s, out var ptr);
            json = TakeUtf8(ptr);
            return (NativeStatusCode)rc;
        }
        catch (Exception ex) when (IsInteropUnavailable(ex))
        {
            json = string.Empty;
            return NativeStatusCode.NativeUnavailable;
        }
    }

    private static bool IsInteropUnavailable(Exception ex) => ex is
        DllNotFoundException or
        EntryPointNotFoundException or
        BadImageFormatException or
        TypeInitializationException or
        FileNotFoundException or
        FileLoadException;

    private static string TakeUtf8(IntPtr ptr)
    {
        if (ptr == IntPtr.Zero) return string.Empty;
        var str = Marshal.PtrToStringUTF8(ptr) ?? string.Empty;
        NativeMethods.Free(ptr);
        return str;
    }

    /// <inheritdoc/>
    public NativeStatusCode GetNativeVersion(out int major, out int minor)
    {
        try
        {
            return (NativeStatusCode)NativeMethods.NativeVersion(out major, out minor);
        }
        catch (Exception ex) when (IsInteropUnavailable(ex))
        {
            major = 0;
            minor = 0;
            return NativeStatusCode.NativeUnavailable;
        }
    }
}
