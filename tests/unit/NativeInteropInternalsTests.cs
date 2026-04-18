using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

using Banana.Api.NativeInterop;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Xunit;

namespace Banana.UnitTests;

public sealed class NativeInteropInternalsTests
{
    private static readonly object Sync = new();
    private static bool IsConfigured;

    [Fact]
    public void NativeMethods_CalculateBanana_ReturnsOk()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var nativeMethodsType = GetNativeMethodsType();
        var method = nativeMethodsType.GetMethod("CalculateBanana", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);

        var args = new object?[] { 10, 2, 0 };
        var status = (int)method.Invoke(null, args)!;

        Assert.Equal((int)NativeStatusCode.Ok, status);
        Assert.Equal(150, (int)args[2]!);
    }

    [Fact]
    public void NativeMethods_CalculateBananaWithBreakdown_ReturnsOk()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var nativeMethodsType = GetNativeMethodsType();
        var method = nativeMethodsType.GetMethod("CalculateBananaWithBreakdown", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);

        var args = new object?[] { 10, 2, null };
        var status = (int)method.Invoke(null, args)!;
        var breakdown = Assert.IsType<BananaBreakdownNative>(args[2]);

        Assert.Equal((int)NativeStatusCode.Ok, status);
        Assert.Equal(10, breakdown.Purchases);
        Assert.Equal(2, breakdown.Multiplier);
        Assert.Equal(150, breakdown.Banana);
    }

    [Fact]
    public void NativeMethods_CreateBananaMessage_AllocatesMessageAndCanBeFreed()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var nativeMethodsType = GetNativeMethodsType();
        var createMethod = nativeMethodsType.GetMethod("CreateBananaMessage", BindingFlags.NonPublic | BindingFlags.Static);
        var freeMethod = nativeMethodsType.GetMethod("Free", BindingFlags.NonPublic | BindingFlags.Static);

        Assert.NotNull(createMethod);
        Assert.NotNull(freeMethod);

        var args = new object?[] { 10, 2, IntPtr.Zero };
        var status = (int)createMethod.Invoke(null, args)!;
        var ptr = (nint)(IntPtr)args[2]!;

        Assert.Equal((int)NativeStatusCode.Ok, status);
        Assert.NotEqual(nint.Zero, ptr);

        try
        {
            var bytes = ReadNullTerminatedUtf8(ptr);
            var message = Encoding.UTF8.GetString(bytes);
            Assert.Contains("banana_profile", message, StringComparison.Ordinal);
            Assert.Contains("banana=150", message, StringComparison.Ordinal);
        }
        finally
        {
            freeMethod.Invoke(null, new object?[] { (IntPtr)ptr });
            freeMethod.Invoke(null, new object?[] { IntPtr.Zero });
        }
    }

    [Fact]
    public void NativeBananaClient_PrivateEnsureSuccess_ThrowsForInternalError()
    {
        var method = typeof(NativeBananaClient).GetMethod("EnsureSuccess", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);

        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object?[] { NativeStatusCode.InternalError }));
        Assert.IsType<NativeInteropException>(ex.InnerException);
    }

    [Fact]
    public void NativeBananaClient_PrivateReadNullTerminatedUtf8_ThrowsForZeroPointer()
    {
        var method = typeof(NativeBananaClient).GetMethod("ReadNullTerminatedUtf8", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);

        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object?[] { nint.Zero }));
        Assert.IsType<NativeInteropException>(ex.InnerException);
    }

    [Fact]
    public void NativeMethods_PredictBananaRipeness_ReturnsOk()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var nativeMethodsType = GetNativeMethodsType();
        var method = nativeMethodsType.GetMethod("PredictBananaRipeness", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);

        var args = new object?[] { new[] { 12.5, 13.0, 14.2 }, 3, 5, 2.5, 0.1, 13.2, null };
        var status = (int)method.Invoke(null, args)!;
        var prediction = Assert.IsType<BananaRipenessPredictionNative>(args[6]);

        Assert.Equal((int)NativeStatusCode.Ok, status);
        Assert.True(Enum.IsDefined(typeof(BananaRipenessStage), prediction.PredictedStage));
        Assert.True(prediction.ShelfLifeHours > 0);
        Assert.True(prediction.RipeningIndex > 0.0);
    }

    [Fact]
    public void NativeMethods_CreateBatch_AndGetBatchStatus_ReturnOk()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var nativeMethodsType = GetNativeMethodsType();
        var createMethod = nativeMethodsType.GetMethod("CreateBatch", BindingFlags.NonPublic | BindingFlags.Static);
        var getMethod = nativeMethodsType.GetMethod("GetBatchStatus", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(createMethod);
        Assert.NotNull(getMethod);

        var createArgs = new object?[] { "batch-1", "farm-1", 13.2, 2.5, 3, IntPtr.Zero };
        var createStatus = (int)createMethod.Invoke(null, createArgs)!;
        var payloadPtr = (nint)(IntPtr)createArgs[5]!;

        Assert.Equal((int)NativeStatusCode.Ok, createStatus);
        Assert.NotEqual(nint.Zero, payloadPtr);

        var getArgs = new object?[] { "batch-1", IntPtr.Zero };
        var getStatus = (int)getMethod.Invoke(null, getArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, getStatus);

        var freeMethod = nativeMethodsType.GetMethod("Free", BindingFlags.NonPublic | BindingFlags.Static);
        freeMethod!.Invoke(null, new object?[] { (IntPtr)payloadPtr });
        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)getArgs[1]! });
    }

    [Fact]
    public void NativeMethods_CreateHarvestBatch_AddBunch_AndGetHarvestBatchStatus_ReturnOk()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var nativeMethodsType = GetNativeMethodsType();
        var createMethod = nativeMethodsType.GetMethod("CreateHarvestBatch", BindingFlags.NonPublic | BindingFlags.Static);
        var addMethod = nativeMethodsType.GetMethod("AddBunchToHarvestBatch", BindingFlags.NonPublic | BindingFlags.Static);
        var getMethod = nativeMethodsType.GetMethod("GetHarvestBatchStatus", BindingFlags.NonPublic | BindingFlags.Static);
        var freeMethod = nativeMethodsType.GetMethod("Free", BindingFlags.NonPublic | BindingFlags.Static);

        Assert.NotNull(createMethod);
        Assert.NotNull(addMethod);
        Assert.NotNull(getMethod);
        Assert.NotNull(freeMethod);

        var createArgs = new object?[] { "harvest-1", "field-7", 42, IntPtr.Zero };
        var createStatus = (int)createMethod.Invoke(null, createArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, createStatus);

        var addArgs = new object?[] { "harvest-1", "bunch-1", 42, 18.5, IntPtr.Zero };
        var addStatus = (int)addMethod.Invoke(null, addArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, addStatus);

        var getArgs = new object?[] { "harvest-1", IntPtr.Zero };
        var getStatus = (int)getMethod.Invoke(null, getArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, getStatus);

        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)createArgs[3]! });
        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)addArgs[4]! });
        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)getArgs[1]! });
    }

    [Fact]
    public void NativeMethods_RegisterTruck_LoadRelocateUnload_AndGetTruckStatus_ReturnOk()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        var nativeMethodsType = GetNativeMethodsType();
        var registerMethod = nativeMethodsType.GetMethod("RegisterTruck", BindingFlags.NonPublic | BindingFlags.Static);
        var loadMethod = nativeMethodsType.GetMethod("LoadTruckContainer", BindingFlags.NonPublic | BindingFlags.Static);
        var relocateMethod = nativeMethodsType.GetMethod("RelocateTruck", BindingFlags.NonPublic | BindingFlags.Static);
        var unloadMethod = nativeMethodsType.GetMethod("UnloadTruckContainer", BindingFlags.NonPublic | BindingFlags.Static);
        var getMethod = nativeMethodsType.GetMethod("GetTruckStatus", BindingFlags.NonPublic | BindingFlags.Static);
        var freeMethod = nativeMethodsType.GetMethod("Free", BindingFlags.NonPublic | BindingFlags.Static);

        Assert.NotNull(registerMethod);
        Assert.NotNull(loadMethod);
        Assert.NotNull(relocateMethod);
        Assert.NotNull(unloadMethod);
        Assert.NotNull(getMethod);
        Assert.NotNull(freeMethod);

        var registerArgs = new object?[]
        {
            "truck-1",
            "warehouse-1",
            BananaDistributionNodeType.Warehouse,
            9.90,
            -79.60,
            60.0,
            IntPtr.Zero
        };
        var registerStatus = (int)registerMethod.Invoke(null, registerArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, registerStatus);

        var loadArgs = new object?[] { "truck-1", "container-1", 18.5, IntPtr.Zero };
        var loadStatus = (int)loadMethod.Invoke(null, loadArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, loadStatus);

        var relocateArgs = new object?[]
        {
            "truck-1",
            "port-3",
            BananaDistributionNodeType.Port,
            8.95,
            -79.55,
            IntPtr.Zero
        };
        var relocateStatus = (int)relocateMethod.Invoke(null, relocateArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, relocateStatus);

        var unloadArgs = new object?[] { "truck-1", "container-1", 18.5, IntPtr.Zero };
        var unloadStatus = (int)unloadMethod.Invoke(null, unloadArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, unloadStatus);

        var getArgs = new object?[] { "truck-1", IntPtr.Zero };
        var getStatus = (int)getMethod.Invoke(null, getArgs)!;
        Assert.Equal((int)NativeStatusCode.Ok, getStatus);

        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)registerArgs[6]! });
        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)loadArgs[3]! });
        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)relocateArgs[5]! });
        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)unloadArgs[3]! });
        freeMethod.Invoke(null, new object?[] { (IntPtr)(nint)(IntPtr)getArgs[1]! });
    }

    private static Type GetNativeMethodsType()
    {
        return typeof(NativeLibraryResolver).Assembly.GetType("Banana.Api.NativeInterop.NativeMethods")
            ?? throw new InvalidOperationException("NativeMethods type not found.");
    }

    private static byte[] ReadNullTerminatedUtf8(nint messagePtr)
    {
        var bytes = new List<byte>();
        var offset = 0;

        while (true)
        {
            var value = Marshal.ReadByte(messagePtr, offset);
            if (value == 0)
            {
                break;
            }

            bytes.Add(value);
            offset++;
        }

        return bytes.ToArray();
    }

    private static bool EnsureNativePathConfigured()
    {
        lock (Sync)
        {
            if (IsConfigured)
            {
                return true;
            }

            var root = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "../../../../../../"));
            var fileName = NativeLibraryResolver.GetPlatformLibraryName();
            var candidates = new[]
            {
                Path.Combine(root, "build", "native", "bin", "Release"),
                Path.Combine(root, "build", "native", "bin")
            };

            var libraryDir = candidates.FirstOrDefault(path => File.Exists(Path.Combine(path, fileName)));
            if (libraryDir is null)
            {
                return false;
            }

            Environment.SetEnvironmentVariable("BANANA_NATIVE_PATH", libraryDir);

            var configuration = new ConfigurationBuilder().Build();
            using var loggerFactory = LoggerFactory.Create(_ => { });
            var logger = loggerFactory.CreateLogger("NativeInteropInternalsTests");

            try
            {
                NativeLibraryResolver.EnsureConfigured(configuration, logger);
            }
            catch (InvalidOperationException ex) when (ex.Message.Contains("resolver is already set", StringComparison.OrdinalIgnoreCase))
            {
            }

            IsConfigured = true;
            return true;
        }
    }
}
