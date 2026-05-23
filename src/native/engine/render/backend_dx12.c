#include "backend_dx12.h"

#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
#define COBJMACROS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#define BANANA_DX12_BACK_BUFFER_COUNT 2

static const char *BANANA_DX12_PROBE_WINDOW_CLASS = "BananaDx12ProbeWindow";

static LRESULT CALLBACK banana_dx12_probe_window_proc(HWND hwnd,
                                                      UINT message,
                                                      WPARAM w_param,
                                                      LPARAM l_param)
{
    return DefWindowProcA(hwnd, message, w_param, l_param);
}

static int banana_dx12_create_probe_window(HINSTANCE instance,
                                           WNDCLASSA *window_class,
                                           HWND *window)
{
    ATOM class_atom = 0;
    HWND probe_window = NULL;

    ZeroMemory(window_class, sizeof(*window_class));
    window_class->style = CS_OWNDC;
    window_class->lpfnWndProc = banana_dx12_probe_window_proc;
    window_class->hInstance = instance;
    window_class->lpszClassName = BANANA_DX12_PROBE_WINDOW_CLASS;

    class_atom = RegisterClassA(window_class);
    if (class_atom == 0)
    {
        return 0;
    }

    probe_window = CreateWindowExA(0,
                                   BANANA_DX12_PROBE_WINDOW_CLASS,
                                   "Banana DX12 Probe",
                                   WS_OVERLAPPEDWINDOW,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   16,
                                   16,
                                   NULL,
                                   NULL,
                                   instance,
                                   NULL);
    if (!probe_window)
    {
        UnregisterClassA(BANANA_DX12_PROBE_WINDOW_CLASS, instance);
        return 0;
    }

    *window = probe_window;
    return 1;
}

typedef struct BananaDx12Runtime
{
    int active;
    int width;
    int height;
    const char *status;
    int owns_window;
    HWND window;
    IDXGIFactory4 *factory;
    IDXGIAdapter1 *adapter;
    ID3D12Device *device;
    ID3D12CommandQueue *command_queue;
    IDXGISwapChain3 *swap_chain;
    ID3D12DescriptorHeap *rtv_heap;
    ID3D12DescriptorHeap *dsv_heap;
    ID3D12Resource *render_targets[BANANA_DX12_BACK_BUFFER_COUNT];
    ID3D12Resource *depth_stencil;
    ID3D12CommandAllocator *command_allocator;
    ID3D12GraphicsCommandList *command_list;
    ID3D12Fence *fence;
    HANDLE fence_event;
    UINT rtv_descriptor_size;
    UINT frame_index;
    UINT64 fence_value;
    UINT present_interval;
    UINT64 frame_counter;
    UINT64 frames_presented;
    HRESULT last_present_result;
    int command_list_recording;
} BananaDx12Runtime;

static BananaDx12Runtime s_dx12_runtime = {0};
static char s_dx12_telemetry[256] = "dx12-runtime-inactive";

static int banana_dx12_runtime_read_present_interval(void)
{
    const char *present_interval_env = getenv("BANANA_DX12_PRESENT_INTERVAL");
    const char *vsync_env = getenv("BANANA_DX12_VSYNC");

    if (present_interval_env && present_interval_env[0])
    {
        long value = strtol(present_interval_env, NULL, 10);
        return value <= 0 ? 0 : 1;
    }
    if (vsync_env && vsync_env[0])
    {
        long value = strtol(vsync_env, NULL, 10);
        return value == 0 ? 0 : 1;
    }

    return 1;
}

static void banana_dx12_runtime_update_telemetry(void)
{
    snprintf(s_dx12_telemetry,
             sizeof(s_dx12_telemetry),
             "status=%s active=%d size=%dx%d frame=%llu presented=%llu interval=%u hr=0x%08lx",
             s_dx12_runtime.status ? s_dx12_runtime.status : "unknown",
             s_dx12_runtime.active,
             s_dx12_runtime.width,
             s_dx12_runtime.height,
             (unsigned long long)s_dx12_runtime.frame_counter,
             (unsigned long long)s_dx12_runtime.frames_presented,
             s_dx12_runtime.present_interval,
             (unsigned long)s_dx12_runtime.last_present_result);
}

static void banana_dx12_runtime_release_depth_target(void)
{
    if (s_dx12_runtime.depth_stencil)
    {
        ID3D12Resource_Release(s_dx12_runtime.depth_stencil);
        s_dx12_runtime.depth_stencil = NULL;
    }
}

static void banana_dx12_runtime_release_swapchain_buffers(void)
{
    int i = 0;
    for (i = 0; i < BANANA_DX12_BACK_BUFFER_COUNT; i++)
    {
        if (s_dx12_runtime.render_targets[i])
        {
            ID3D12Resource_Release(s_dx12_runtime.render_targets[i]);
            s_dx12_runtime.render_targets[i] = NULL;
        }
    }
    banana_dx12_runtime_release_depth_target();
}

static void banana_dx12_runtime_wait_for_gpu(void)
{
    UINT64 fence_target = 0;
    if (!s_dx12_runtime.command_queue || !s_dx12_runtime.fence || !s_dx12_runtime.fence_event)
    {
        return;
    }

    s_dx12_runtime.fence_value++;
    fence_target = s_dx12_runtime.fence_value;
    if (FAILED(ID3D12CommandQueue_Signal(s_dx12_runtime.command_queue,
                                         (ID3D12Fence *)s_dx12_runtime.fence,
                                         fence_target)))
    {
        return;
    }

    if (ID3D12Fence_GetCompletedValue(s_dx12_runtime.fence) < fence_target)
    {
        if (SUCCEEDED(ID3D12Fence_SetEventOnCompletion(s_dx12_runtime.fence,
                                                       fence_target,
                                                       s_dx12_runtime.fence_event)))
        {
            WaitForSingleObject(s_dx12_runtime.fence_event, INFINITE);
        }
    }
}

static int banana_dx12_runtime_create_rtv_targets(void)
{
    UINT i = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE handle;

    if (!s_dx12_runtime.swap_chain || !s_dx12_runtime.rtv_heap || !s_dx12_runtime.device)
    {
        return 0;
    }

    banana_dx12_runtime_release_swapchain_buffers();

    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dx12_runtime.rtv_heap, &handle);
    for (i = 0; i < BANANA_DX12_BACK_BUFFER_COUNT; i++)
    {
        IDXGISwapChain3_GetBuffer(s_dx12_runtime.swap_chain,
                                  i,
                                  &IID_ID3D12Resource,
                                  (void **)&s_dx12_runtime.render_targets[i]);
        if (!s_dx12_runtime.render_targets[i])
        {
            banana_dx12_runtime_release_swapchain_buffers();
            return 0;
        }
        ID3D12Device_CreateRenderTargetView(s_dx12_runtime.device,
                                            s_dx12_runtime.render_targets[i],
                                            NULL,
                                            handle);
        handle.ptr += s_dx12_runtime.rtv_descriptor_size;
    }

    s_dx12_runtime.frame_index = IDXGISwapChain3_GetCurrentBackBufferIndex(s_dx12_runtime.swap_chain);
    return 1;
}

static int banana_dx12_runtime_create_depth_target(void)
{
    D3D12_RESOURCE_DESC depth_desc;
    D3D12_HEAP_PROPERTIES heap_props;
    D3D12_CLEAR_VALUE clear_value;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;
    HRESULT result;

    if (!s_dx12_runtime.device || !s_dx12_runtime.dsv_heap ||
        s_dx12_runtime.width <= 0 || s_dx12_runtime.height <= 0)
    {
        return 0;
    }

    banana_dx12_runtime_release_depth_target();

    ZeroMemory(&depth_desc, sizeof(depth_desc));
    depth_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depth_desc.Alignment = 0;
    depth_desc.Width = (UINT64)s_dx12_runtime.width;
    depth_desc.Height = (UINT)s_dx12_runtime.height;
    depth_desc.DepthOrArraySize = 1;
    depth_desc.MipLevels = 1;
    depth_desc.Format = DXGI_FORMAT_D32_FLOAT;
    depth_desc.SampleDesc.Count = 1;
    depth_desc.SampleDesc.Quality = 0;
    depth_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depth_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    ZeroMemory(&heap_props, sizeof(heap_props));
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask = 1;
    heap_props.VisibleNodeMask = 1;

    ZeroMemory(&clear_value, sizeof(clear_value));
    clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0;

    result = ID3D12Device_CreateCommittedResource(s_dx12_runtime.device,
                                                  &heap_props,
                                                  D3D12_HEAP_FLAG_NONE,
                                                  &depth_desc,
                                                  D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                  &clear_value,
                                                  &IID_ID3D12Resource,
                                                  (void **)&s_dx12_runtime.depth_stencil);
    if (FAILED(result) || !s_dx12_runtime.depth_stencil)
    {
        return 0;
    }

    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dx12_runtime.dsv_heap,
                                                            &dsv_handle);
    ID3D12Device_CreateDepthStencilView(s_dx12_runtime.device,
                                        s_dx12_runtime.depth_stencil,
                                        &dsv_desc,
                                        dsv_handle);
    return 1;
}

static int banana_dx12_runtime_recreate_swap_chain(int width, int height)
{
    IDXGISwapChain1 *swap_chain1 = NULL;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    HRESULT result;

    if (!s_dx12_runtime.factory || !s_dx12_runtime.command_queue || !s_dx12_runtime.window)
        return 0;

    if (s_dx12_runtime.swap_chain)
    {
        IDXGISwapChain3_Release(s_dx12_runtime.swap_chain);
        s_dx12_runtime.swap_chain = NULL;
    }

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
    swap_chain_desc.Width = (UINT)width;
    swap_chain_desc.Height = (UINT)height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = BANANA_DX12_BACK_BUFFER_COUNT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    result = IDXGIFactory4_CreateSwapChainForHwnd(s_dx12_runtime.factory,
                                                  (IUnknown *)s_dx12_runtime.command_queue,
                                                  s_dx12_runtime.window,
                                                  &swap_chain_desc,
                                                  NULL,
                                                  NULL,
                                                  &swap_chain1);
    if (FAILED(result) || !swap_chain1)
        return 0;

    result = IDXGISwapChain1_QueryInterface(swap_chain1,
                                            &IID_IDXGISwapChain3,
                                            (void **)&s_dx12_runtime.swap_chain);
    IDXGISwapChain1_Release(swap_chain1);
    if (FAILED(result) || !s_dx12_runtime.swap_chain)
        return 0;

    IDXGIFactory4_MakeWindowAssociation(s_dx12_runtime.factory,
                                        s_dx12_runtime.window,
                                        DXGI_MWA_NO_ALT_ENTER);
    s_dx12_runtime.width = width;
    s_dx12_runtime.height = height;
    return 1;
}

static void banana_dx12_runtime_reset_state(void)
{
    ZeroMemory(&s_dx12_runtime, sizeof(s_dx12_runtime));
    s_dx12_runtime.status = "dx12-runtime-inactive";
    s_dx12_runtime.last_present_result = S_OK;
    s_dx12_runtime.present_interval = 1;
    banana_dx12_runtime_update_telemetry();
}

static void banana_dx12_runtime_release_all(void)
{
    banana_dx12_runtime_wait_for_gpu();
    banana_dx12_runtime_release_swapchain_buffers();

    if (s_dx12_runtime.fence_event)
    {
        CloseHandle(s_dx12_runtime.fence_event);
        s_dx12_runtime.fence_event = NULL;
    }
    if (s_dx12_runtime.fence)
    {
        ID3D12Fence_Release(s_dx12_runtime.fence);
        s_dx12_runtime.fence = NULL;
    }
    if (s_dx12_runtime.command_list)
    {
        ID3D12GraphicsCommandList_Release(s_dx12_runtime.command_list);
        s_dx12_runtime.command_list = NULL;
    }
    if (s_dx12_runtime.command_allocator)
    {
        ID3D12CommandAllocator_Release(s_dx12_runtime.command_allocator);
        s_dx12_runtime.command_allocator = NULL;
    }
    if (s_dx12_runtime.rtv_heap)
    {
        ID3D12DescriptorHeap_Release(s_dx12_runtime.rtv_heap);
        s_dx12_runtime.rtv_heap = NULL;
    }
    if (s_dx12_runtime.dsv_heap)
    {
        ID3D12DescriptorHeap_Release(s_dx12_runtime.dsv_heap);
        s_dx12_runtime.dsv_heap = NULL;
    }
    if (s_dx12_runtime.swap_chain)
    {
        IDXGISwapChain3_Release(s_dx12_runtime.swap_chain);
        s_dx12_runtime.swap_chain = NULL;
    }
    if (s_dx12_runtime.command_queue)
    {
        ID3D12CommandQueue_Release(s_dx12_runtime.command_queue);
        s_dx12_runtime.command_queue = NULL;
    }
    if (s_dx12_runtime.device)
    {
        ID3D12Device_Release(s_dx12_runtime.device);
        s_dx12_runtime.device = NULL;
    }
    if (s_dx12_runtime.adapter)
    {
        IDXGIAdapter1_Release(s_dx12_runtime.adapter);
        s_dx12_runtime.adapter = NULL;
    }
    if (s_dx12_runtime.factory)
    {
        IDXGIFactory4_Release(s_dx12_runtime.factory);
        s_dx12_runtime.factory = NULL;
    }
    if (s_dx12_runtime.window && s_dx12_runtime.owns_window)
    {
        DestroyWindow(s_dx12_runtime.window);
        s_dx12_runtime.window = NULL;
    }
    s_dx12_runtime.window = NULL;
    s_dx12_runtime.owns_window = 0;

    banana_dx12_runtime_reset_state();
}

static IDXGIAdapter1 *banana_dx12_find_hardware_adapter(IDXGIFactory4 *factory)
{
    UINT adapter_index = 0;
    IDXGIAdapter1 *adapter = NULL;

    while (IDXGIFactory4_EnumAdapters1(factory, adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 description;
        HRESULT result = IDXGIAdapter1_GetDesc1(adapter, &description);
        if (FAILED(result))
        {
            IDXGIAdapter1_Release(adapter);
            adapter = NULL;
            adapter_index++;
            continue;
        }

        if ((description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
        {
            IDXGIAdapter1_Release(adapter);
            adapter = NULL;
            adapter_index++;
            continue;
        }

        return adapter;
    }

    return NULL;
}
#endif

enum
{
    BANANA_DX12_PROBE_UNKNOWN = 0,
    BANANA_DX12_PROBE_AVAILABLE = 1,
    BANANA_DX12_PROBE_UNAVAILABLE = 2,
};

static int s_dx12_probe_state = BANANA_DX12_PROBE_UNKNOWN;
static const char *s_dx12_probe_status = "dx12-not-probed";

static void banana_dx12_probe_once(void)
{
    if (s_dx12_probe_state != BANANA_DX12_PROBE_UNKNOWN)
    {
        return;
    }

#if !defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
    s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
    s_dx12_probe_status = "dx12-not-requested";
#elif !defined(_WIN32)
    s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
    s_dx12_probe_status = "dx12-non-windows-host";
#else
    HINSTANCE instance = GetModuleHandleA(NULL);
    WNDCLASSA window_class;
    HWND window = NULL;
    IDXGIFactory4 *factory = NULL;
    IDXGIAdapter1 *adapter = NULL;
    ID3D12Device *device = NULL;
    ID3D12CommandQueue *command_queue = NULL;
    IDXGISwapChain1 *swap_chain = NULL;
    D3D12_COMMAND_QUEUE_DESC queue_desc;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    HRESULT result;

    if (!instance)
    {
        s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
        s_dx12_probe_status = "dx12-instance-unavailable";
        return;
    }
    if (!banana_dx12_create_probe_window(instance, &window_class, &window))
    {
        s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
        s_dx12_probe_status = "dx12-window-create-failed";
        return;
    }

    result = CreateDXGIFactory2(0,
                                &IID_IDXGIFactory4,
                                (void **)&factory);
    if (FAILED(result) || !factory)
    {
        s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
        s_dx12_probe_status = "dx12-factory-create-failed";
        goto cleanup;
    }

    adapter = banana_dx12_find_hardware_adapter(factory);
    if (!adapter)
    {
        s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
        s_dx12_probe_status = "dx12-adapter-not-found";
        goto cleanup;
    }

    result = D3D12CreateDevice((IUnknown *)adapter,
                               D3D_FEATURE_LEVEL_11_0,
                               &IID_ID3D12Device,
                               (void **)&device);
    if (FAILED(result) || !device)
    {
        s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
        s_dx12_probe_status = "dx12-device-create-failed";
        goto cleanup;
    }

    ZeroMemory(&queue_desc, sizeof(queue_desc));
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.NodeMask = 0;

    result = ID3D12Device_CreateCommandQueue(device,
                                             &queue_desc,
                                             &IID_ID3D12CommandQueue,
                                             (void **)&command_queue);
    if (FAILED(result) || !command_queue)
    {
        s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
        s_dx12_probe_status = "dx12-command-queue-create-failed";
        goto cleanup;
    }

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
    swap_chain_desc.Width = 16;
    swap_chain_desc.Height = 16;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.Stereo = FALSE;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_desc.Flags = 0;

    result = IDXGIFactory4_CreateSwapChainForHwnd(factory,
                                                  (IUnknown *)command_queue,
                                                  window,
                                                  &swap_chain_desc,
                                                  NULL,
                                                  NULL,
                                                  &swap_chain);
    if (FAILED(result) || !swap_chain)
    {
        s_dx12_probe_state = BANANA_DX12_PROBE_UNAVAILABLE;
        s_dx12_probe_status = "dx12-swap-chain-create-failed";
        goto cleanup;
    }

    IDXGIFactory4_MakeWindowAssociation(factory, window, DXGI_MWA_NO_ALT_ENTER);
    s_dx12_probe_state = BANANA_DX12_PROBE_AVAILABLE;
    s_dx12_probe_status = "dx12-bootstrap-ready";

cleanup:
    if (swap_chain)
        IDXGISwapChain1_Release(swap_chain);
    if (command_queue)
        ID3D12CommandQueue_Release(command_queue);
    if (device)
        ID3D12Device_Release(device);
    if (adapter)
        IDXGIAdapter1_Release(adapter);
    if (factory)
        IDXGIFactory4_Release(factory);
    DestroyWindow(window);
    UnregisterClassA(BANANA_DX12_PROBE_WINDOW_CLASS, instance);
#endif
}

int banana_dx12_backend_is_available(void)
{
    banana_dx12_probe_once();
    return s_dx12_probe_state == BANANA_DX12_PROBE_AVAILABLE;
}

const char *banana_dx12_backend_status(void)
{
    if (s_dx12_runtime.active)
    {
        return s_dx12_runtime.status;
    }
    banana_dx12_probe_once();
    return s_dx12_probe_status;
}

const char *banana_dx12_runtime_telemetry(void)
{
    banana_dx12_runtime_update_telemetry();
    return s_dx12_telemetry;
}

unsigned long long banana_dx12_runtime_frames_presented(void)
{
    return (unsigned long long)s_dx12_runtime.frames_presented;
}

int banana_dx12_runtime_init(void *native_window, int width, int height)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    IDXGISwapChain1 *swap_chain1 = NULL;
    D3D12_COMMAND_QUEUE_DESC queue_desc;
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    HRESULT result;

    if (s_dx12_runtime.active)
    {
        if (width == s_dx12_runtime.width && height == s_dx12_runtime.height)
        {
            return 1;
        }
        if (!banana_dx12_runtime_resize(width, height))
        {
            s_dx12_runtime.status = "dx12-runtime-resize-failed";
            return 0;
        }
        return 1;
    }

    banana_dx12_runtime_reset_state();
    if (!native_window)
    {
        s_dx12_runtime.status = "dx12-native-window-missing";
        return 0;
    }
    s_dx12_runtime.window = (HWND)native_window;
    s_dx12_runtime.owns_window = 0;
    s_dx12_runtime.present_interval = (UINT)banana_dx12_runtime_read_present_interval();

    result = CreateDXGIFactory2(0, &IID_IDXGIFactory4, (void **)&s_dx12_runtime.factory);
    if (FAILED(result) || !s_dx12_runtime.factory)
    {
        s_dx12_runtime.status = "dx12-factory-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    s_dx12_runtime.adapter = banana_dx12_find_hardware_adapter(s_dx12_runtime.factory);
    if (!s_dx12_runtime.adapter)
    {
        s_dx12_runtime.status = "dx12-adapter-not-found";
        banana_dx12_runtime_release_all();
        return 0;
    }

    result = D3D12CreateDevice((IUnknown *)s_dx12_runtime.adapter,
                               D3D_FEATURE_LEVEL_11_0,
                               &IID_ID3D12Device,
                               (void **)&s_dx12_runtime.device);
    if (FAILED(result) || !s_dx12_runtime.device)
    {
        s_dx12_runtime.status = "dx12-device-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    ZeroMemory(&queue_desc, sizeof(queue_desc));
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    result = ID3D12Device_CreateCommandQueue(s_dx12_runtime.device,
                                             &queue_desc,
                                             &IID_ID3D12CommandQueue,
                                             (void **)&s_dx12_runtime.command_queue);
    if (FAILED(result) || !s_dx12_runtime.command_queue)
    {
        s_dx12_runtime.status = "dx12-command-queue-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
    swap_chain_desc.Width = (UINT)width;
    swap_chain_desc.Height = (UINT)height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = BANANA_DX12_BACK_BUFFER_COUNT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    result = IDXGIFactory4_CreateSwapChainForHwnd(s_dx12_runtime.factory,
                                                  (IUnknown *)s_dx12_runtime.command_queue,
                                                  s_dx12_runtime.window,
                                                  &swap_chain_desc,
                                                  NULL,
                                                  NULL,
                                                  &swap_chain1);
    if (FAILED(result) || !swap_chain1)
    {
        s_dx12_runtime.status = "dx12-swap-chain-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    result = IDXGISwapChain1_QueryInterface(swap_chain1,
                                            &IID_IDXGISwapChain3,
                                            (void **)&s_dx12_runtime.swap_chain);
    IDXGISwapChain1_Release(swap_chain1);
    if (FAILED(result) || !s_dx12_runtime.swap_chain)
    {
        s_dx12_runtime.status = "dx12-swap-chain-query-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    IDXGIFactory4_MakeWindowAssociation(s_dx12_runtime.factory,
                                        s_dx12_runtime.window,
                                        DXGI_MWA_NO_ALT_ENTER);

    ZeroMemory(&rtv_heap_desc, sizeof(rtv_heap_desc));
    rtv_heap_desc.NumDescriptors = BANANA_DX12_BACK_BUFFER_COUNT;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    result = ID3D12Device_CreateDescriptorHeap(s_dx12_runtime.device,
                                               &rtv_heap_desc,
                                               &IID_ID3D12DescriptorHeap,
                                               (void **)&s_dx12_runtime.rtv_heap);
    if (FAILED(result) || !s_dx12_runtime.rtv_heap)
    {
        s_dx12_runtime.status = "dx12-rtv-heap-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    s_dx12_runtime.rtv_descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(
        s_dx12_runtime.device,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    ZeroMemory(&dsv_heap_desc, sizeof(dsv_heap_desc));
    dsv_heap_desc.NumDescriptors = 1;
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    result = ID3D12Device_CreateDescriptorHeap(s_dx12_runtime.device,
                                               &dsv_heap_desc,
                                               &IID_ID3D12DescriptorHeap,
                                               (void **)&s_dx12_runtime.dsv_heap);
    if (FAILED(result) || !s_dx12_runtime.dsv_heap)
    {
        s_dx12_runtime.status = "dx12-dsv-heap-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    if (!banana_dx12_runtime_create_rtv_targets())
    {
        s_dx12_runtime.status = "dx12-render-target-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    if (!banana_dx12_runtime_create_depth_target())
    {
        s_dx12_runtime.status = "dx12-depth-target-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    result = ID3D12Device_CreateCommandAllocator(s_dx12_runtime.device,
                                                 D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                 &IID_ID3D12CommandAllocator,
                                                 (void **)&s_dx12_runtime.command_allocator);
    if (FAILED(result) || !s_dx12_runtime.command_allocator)
    {
        s_dx12_runtime.status = "dx12-command-allocator-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    result = ID3D12Device_CreateCommandList(s_dx12_runtime.device,
                                            0,
                                            D3D12_COMMAND_LIST_TYPE_DIRECT,
                                            s_dx12_runtime.command_allocator,
                                            NULL,
                                            &IID_ID3D12GraphicsCommandList,
                                            (void **)&s_dx12_runtime.command_list);
    if (FAILED(result) || !s_dx12_runtime.command_list)
    {
        s_dx12_runtime.status = "dx12-command-list-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    ID3D12GraphicsCommandList_Close(s_dx12_runtime.command_list);

    result = ID3D12Device_CreateFence(s_dx12_runtime.device,
                                      0,
                                      D3D12_FENCE_FLAG_NONE,
                                      &IID_ID3D12Fence,
                                      (void **)&s_dx12_runtime.fence);
    if (FAILED(result) || !s_dx12_runtime.fence)
    {
        s_dx12_runtime.status = "dx12-fence-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    s_dx12_runtime.fence_value = 0;
    s_dx12_runtime.fence_event = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (!s_dx12_runtime.fence_event)
    {
        s_dx12_runtime.status = "dx12-fence-event-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    s_dx12_runtime.width = width;
    s_dx12_runtime.height = height;
    s_dx12_runtime.active = 1;
    s_dx12_runtime.status = "dx12-runtime-ready";
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    (void)native_window;
    (void)width;
    (void)height;
    return 0;
#endif
}

int banana_dx12_runtime_resize(int width, int height)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    HRESULT result;
    if (!s_dx12_runtime.active || !s_dx12_runtime.swap_chain)
    {
        return 0;
    }

    if (width <= 0 || height <= 0)
        return 1;

    banana_dx12_runtime_wait_for_gpu();
    banana_dx12_runtime_release_swapchain_buffers();

    result = IDXGISwapChain3_ResizeBuffers(s_dx12_runtime.swap_chain,
                                           BANANA_DX12_BACK_BUFFER_COUNT,
                                           (UINT)width,
                                           (UINT)height,
                                           DXGI_FORMAT_R8G8B8A8_UNORM,
                                           0);
    if (FAILED(result))
    {
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
        {
            if (banana_dx12_runtime_recreate_swap_chain(width, height) &&
                banana_dx12_runtime_create_rtv_targets() &&
                banana_dx12_runtime_create_depth_target())
            {
                s_dx12_runtime.status = "dx12-runtime-recovered";
                banana_dx12_runtime_update_telemetry();
                return 1;
            }
            s_dx12_runtime.status = "dx12-recreate-after-loss-failed";
            banana_dx12_runtime_update_telemetry();
            return 0;
        }
        s_dx12_runtime.status = "dx12-resize-buffers-failed";
        banana_dx12_runtime_update_telemetry();
        return 0;
    }
    s_dx12_runtime.width = width;
    s_dx12_runtime.height = height;

    if (!banana_dx12_runtime_create_rtv_targets())
    {
        s_dx12_runtime.status = "dx12-resize-targets-failed";
        banana_dx12_runtime_update_telemetry();
        return 0;
    }
    if (!banana_dx12_runtime_create_depth_target())
    {
        s_dx12_runtime.status = "dx12-depth-target-resize-failed";
        banana_dx12_runtime_update_telemetry();
        return 0;
    }
    s_dx12_runtime.status = "dx12-runtime-ready";
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    (void)width;
    (void)height;
    return 0;
#endif
}

int banana_dx12_runtime_begin_frame(float clear_r, float clear_g, float clear_b, float clear_a)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    D3D12_RESOURCE_BARRIER transition_to_rtv;
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissor_rect;
    float color[4] = {clear_r, clear_g, clear_b, clear_a};

    if (!s_dx12_runtime.active || !s_dx12_runtime.command_allocator || !s_dx12_runtime.command_list ||
        !s_dx12_runtime.render_targets[s_dx12_runtime.frame_index] || !s_dx12_runtime.depth_stencil)
    {
        return 0;
    }

    if (FAILED(ID3D12CommandAllocator_Reset(s_dx12_runtime.command_allocator)))
    {
        s_dx12_runtime.status = "dx12-command-allocator-reset-failed";
        return 0;
    }
    if (FAILED(ID3D12GraphicsCommandList_Reset(s_dx12_runtime.command_list,
                                               s_dx12_runtime.command_allocator,
                                               NULL)))
    {
        s_dx12_runtime.status = "dx12-command-list-reset-failed";
        return 0;
    }

    ZeroMemory(&transition_to_rtv, sizeof(transition_to_rtv));
    transition_to_rtv.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    transition_to_rtv.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    transition_to_rtv.Transition.pResource = s_dx12_runtime.render_targets[s_dx12_runtime.frame_index];
    transition_to_rtv.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    transition_to_rtv.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition_to_rtv.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ID3D12GraphicsCommandList_ResourceBarrier(s_dx12_runtime.command_list, 1, &transition_to_rtv);

    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dx12_runtime.rtv_heap,
                                                            &rtv_handle);
    rtv_handle.ptr += ((SIZE_T)s_dx12_runtime.frame_index * s_dx12_runtime.rtv_descriptor_size);
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dx12_runtime.dsv_heap,
                                                            &dsv_handle);

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (float)s_dx12_runtime.width;
    viewport.Height = (float)s_dx12_runtime.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    scissor_rect.left = 0;
    scissor_rect.top = 0;
    scissor_rect.right = s_dx12_runtime.width;
    scissor_rect.bottom = s_dx12_runtime.height;

    ID3D12GraphicsCommandList_RSSetViewports(s_dx12_runtime.command_list, 1, &viewport);
    ID3D12GraphicsCommandList_RSSetScissorRects(s_dx12_runtime.command_list, 1, &scissor_rect);
    ID3D12GraphicsCommandList_OMSetRenderTargets(s_dx12_runtime.command_list,
                                                 1,
                                                 &rtv_handle,
                                                 FALSE,
                                                 &dsv_handle);

    ID3D12GraphicsCommandList_ClearRenderTargetView(s_dx12_runtime.command_list,
                                                    rtv_handle,
                                                    color,
                                                    0,
                                                    NULL);
    ID3D12GraphicsCommandList_ClearDepthStencilView(s_dx12_runtime.command_list,
                                                    dsv_handle,
                                                    D3D12_CLEAR_FLAG_DEPTH,
                                                    1.0f,
                                                    0,
                                                    0,
                                                    NULL);

    s_dx12_runtime.command_list_recording = 1;
    s_dx12_runtime.frame_counter += 1;
    s_dx12_runtime.status = "dx12-frame-recording";
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    (void)clear_r;
    (void)clear_g;
    (void)clear_b;
    (void)clear_a;
    return 0;
#endif
}

int banana_dx12_runtime_end_frame(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    D3D12_RESOURCE_BARRIER transition_to_present;
    ID3D12CommandList *command_lists[1];
    HRESULT result;
    if (!s_dx12_runtime.active || !s_dx12_runtime.swap_chain || !s_dx12_runtime.command_list_recording)
    {
        return 0;
    }

    ZeroMemory(&transition_to_present, sizeof(transition_to_present));
    transition_to_present.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    transition_to_present.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    transition_to_present.Transition.pResource = s_dx12_runtime.render_targets[s_dx12_runtime.frame_index];
    transition_to_present.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition_to_present.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    transition_to_present.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ID3D12GraphicsCommandList_ResourceBarrier(s_dx12_runtime.command_list, 1, &transition_to_present);

    if (FAILED(ID3D12GraphicsCommandList_Close(s_dx12_runtime.command_list)))
    {
        s_dx12_runtime.status = "dx12-command-list-close-failed";
        s_dx12_runtime.command_list_recording = 0;
        banana_dx12_runtime_update_telemetry();
        return 0;
    }

    command_lists[0] = (ID3D12CommandList *)s_dx12_runtime.command_list;
    ID3D12CommandQueue_ExecuteCommandLists(s_dx12_runtime.command_queue, 1, command_lists);

    result = IDXGISwapChain3_Present(s_dx12_runtime.swap_chain, s_dx12_runtime.present_interval, 0);
    s_dx12_runtime.last_present_result = result;
    if (FAILED(result))
    {
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
        {
            if (banana_dx12_runtime_recreate_swap_chain(s_dx12_runtime.width, s_dx12_runtime.height) &&
                banana_dx12_runtime_create_rtv_targets() &&
                banana_dx12_runtime_create_depth_target())
            {
                s_dx12_runtime.command_list_recording = 0;
                s_dx12_runtime.status = "dx12-runtime-recovered";
                banana_dx12_runtime_update_telemetry();
                return 1;
            }
            s_dx12_runtime.status = "dx12-present-device-loss";
            s_dx12_runtime.command_list_recording = 0;
            banana_dx12_runtime_update_telemetry();
            return 0;
        }
        s_dx12_runtime.status = "dx12-present-failed";
        s_dx12_runtime.command_list_recording = 0;
        banana_dx12_runtime_update_telemetry();
        return 0;
    }

    banana_dx12_runtime_wait_for_gpu();
    s_dx12_runtime.frame_index = IDXGISwapChain3_GetCurrentBackBufferIndex(s_dx12_runtime.swap_chain);
    s_dx12_runtime.command_list_recording = 0;
    s_dx12_runtime.frames_presented += 1;
    s_dx12_runtime.status = "dx12-runtime-ready";
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    return 0;
#endif
}

void banana_dx12_runtime_shutdown(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    banana_dx12_runtime_release_all();
#else
    banana_dx12_runtime_update_telemetry();
#endif
}
