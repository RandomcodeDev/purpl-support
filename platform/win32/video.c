/*++

Copyright (c) 2024 Randomcode Developers

Module Name:

    video.c

Abstract:

    This file implements the Windows video functions.

--*/

#include "common/alloc.h"
#include "common/common.h"
#include "common/configvar.h"

#ifdef PURPL_ENGINE
#include "engine/render/render.h"
#endif

#include "platform/platform.h"
#include "platform/video.h"

// #include "imgui_backends/imgui_impl_win32.h"

#ifdef _MSC_VER
#pragma comment(                                                                                                       \
    linker,                                                                                                            \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define IDI_ICON1 103

static HWND Window = NULL;

static CHAR WindowClassName[] = "PurplWindow";

static PCHAR WindowTitle;

static INT32 WindowWidth;
static INT32 WindowHeight;
static INT32 ExtraWidth;
static INT32 ExtraHeight;

static BOOLEAN WindowResized;
static BOOLEAN WindowFocused;
static BOOLEAN WindowClosed;

static HDC DeviceContext;
#ifdef PURPL_OPENGL
static HGLRC GlContext;
#endif

#ifndef PURPL_XBOX360
static LRESULT CALLBACK WindowProcedure(_In_ HWND MessageWindow, _In_ UINT Message, _In_ WPARAM Wparam,
                                        _In_ LPARAM Lparam)
{
    //    ImGuiIO* Io = igGetIO();

    //    if ( ImGui_ImplWin32_WndProcHandler(
    //            MessageWindow,
    //            Message,
    //            Wparam,
    //            Lparam
    //            ) )
    //    {
    //        return TRUE;
    //    }

    if (MessageWindow == Window)
    {
        switch (Message)
        {
        case WM_SETTEXT:
            CmnFree(WindowTitle);
            WindowTitle = CmnDuplicateString((PCSTR)Lparam, 0);
            break;
        case WM_SIZE: {
            RECT ClientArea = {0};
            INT32 NewWidth;
            INT32 NewHeight;

            GetClientRect(Window, &ClientArea);
            NewWidth = ClientArea.right - ClientArea.left;
            NewHeight = ClientArea.bottom - ClientArea.top;

            if (NewWidth != WindowWidth || NewHeight != WindowHeight)
            {
                WindowResized = TRUE;
                LogInfo("Window resized from %dx%d to %dx%d", WindowWidth, WindowHeight, NewWidth, NewHeight);
            }
            WindowWidth = NewWidth;
            WindowHeight = NewHeight;
            return 0;
        }
        case WM_ACTIVATEAPP:
            WindowFocused = (BOOLEAN)Wparam;
            LogInfo("Window %s", WindowFocused ? "focused" : "unfocused");
            return 0;
        case WM_DESTROY:
        case WM_CLOSE: {
            LogInfo("Window closed");
            WindowClosed = TRUE;
            return 0;
        }
        }
    }

    return DefWindowProcA(MessageWindow, Message, Wparam, Lparam);
}

static VOID RegisterWindowClass(VOID)
{
    WNDCLASSEXA WindowClass;

    LogDebug("Registering window class %s", WindowClassName);

    memset(&WindowClass, 0, sizeof(WindowClass));
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.lpfnWndProc = WindowProcedure;
    WindowClass.hInstance = GetModuleHandleA(NULL);
    WindowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    WindowClass.hIcon = LoadIconA(GetModuleHandleA(NULL), (PCSTR)IDI_ICON1);
    WindowClass.lpszClassName = WindowClassName;
    if (!RegisterClassExA(&WindowClass))
    {
        DWORD Error = GetLastError();
        CmnError("Failed to register window class: error 0x%X (%d)", Error, Error);
    }

    LogDebug("Window class registered");
}

static VOID InitializeWindow(VOID)
{
    RECT ClientArea;
    RECT AdjustedClientArea;

    ClientArea.left = 0;
    ClientArea.right = (UINT32)(GetSystemMetrics(SM_CXSCREEN) / 1.5);
    ClientArea.top = 0;
    ClientArea.bottom = (UINT32)(GetSystemMetrics(SM_CYSCREEN) / 1.5);
    AdjustedClientArea = ClientArea;
    AdjustWindowRect(&AdjustedClientArea, WS_OVERLAPPEDWINDOW, FALSE);
    WindowWidth = AdjustedClientArea.right - AdjustedClientArea.left;
    WindowHeight = AdjustedClientArea.bottom - AdjustedClientArea.top;

    ExtraWidth = ClientArea.left - AdjustedClientArea.left;
    ExtraHeight = ClientArea.top - AdjustedClientArea.top;

    WindowTitle = CmnFormatString(PURPL_NAME " | " PURPL_BUILD_TYPE " | v" PURPL_VERSION_STRING
#ifdef PURPL_DEBUG
                                             " | " PURPL_COMMIT "-" PURPL_BRANCH
#endif
#ifdef PURPL_ENGINE
                                             " | %s renderer",
                                  RdrGetApiName(CONFIGVAR_GET_INT("rdr_api"))
#endif
    );

    LogDebug("Creating %dx%d (for internal size %dx%d) window titled %s", WindowWidth, WindowHeight,
             ClientArea.right - ClientArea.left, ClientArea.bottom - ClientArea.top, WindowTitle);

    Window = CreateWindowExA(0, WindowClassName, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             WindowWidth, WindowHeight, NULL, NULL, GetModuleHandleA(NULL), NULL);
    if (!Window)
    {
        CmnError("Failed to create window: error 0x%X (%d)", GetLastError(), GetLastError());
    }

    GetClientRect(Window, &ClientArea);
    WindowWidth = ClientArea.right - ClientArea.left;
    WindowHeight = ClientArea.bottom - ClientArea.top;

    WindowResized = FALSE;
    WindowFocused = TRUE;
    WindowClosed = FALSE;

    DeviceContext = GetWindowDC(Window);

    LogDebug("Successfully created window with handle 0x%llX", (UINT64)Window);
}

#ifdef PURPL_OPENGL
static HMODULE OpenGl32Handle;

static GLADapiproc GetGlSymbol(_In_z_ PCSTR Name)
{
    // LogDebug("Getting OpenGL symbol %s", Name);
    PROC Symbol = wglGetProcAddress(Name);
    if (!Symbol)
    {
        Symbol = GetProcAddress(OpenGl32Handle, Name);
    }

    return (GLADapiproc)Symbol;
}

static BOOLEAN EnableOpenGl(VOID)
{
    LogDebug("Enabling OpenGL");

    LogDebug("Loading opengl32.dll");
    OpenGl32Handle = LoadLibraryA("opengl32.dll");

    // https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)

    PIXELFORMATDESCRIPTOR DummyPixelFormat = {.nSize = sizeof(PIXELFORMATDESCRIPTOR),
                                              .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                                              .iPixelType = PFD_TYPE_RGBA,
                                              .cColorBits = 32,
                                              .cDepthBits = 24,
                                              .cStencilBits = 8,
                                              0};

    LogDebug("Creating dummy context");

    INT32 ChosenFormat = ChoosePixelFormat(DeviceContext, &DummyPixelFormat);
    SetPixelFormat(DeviceContext, ChosenFormat, &DummyPixelFormat);

    HGLRC DummyContext = wglCreateContext(DeviceContext);
    wglMakeCurrent(DeviceContext, DummyContext);

    gladLoadWGL(DeviceContext, GetGlSymbol);

    CONST INT32 FormatAttributes[] = {
        WGL_DRAW_TO_WINDOW_ARB,
        GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,
        GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,
        GL_TRUE,
        WGL_PIXEL_TYPE_ARB,
        WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,
        32,
        WGL_DEPTH_BITS_ARB,
        24,
        WGL_STENCIL_BITS_ARB,
        8,
        0 // Terminator
    };

    INT32 GoodFormat = 0;
    UINT32 FormatCount;

    wglChoosePixelFormatARB(DeviceContext, FormatAttributes, NULL, 1, &GoodFormat, &FormatCount);

    wglDeleteContext(DummyContext);

    LogDebug("Creating real context");

    DestroyWindow(Window);
    InitializeWindow();

    CONST INT32 ContextAttributes[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        4,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        6,
#ifdef PURPL_DEBUG
        WGL_CONTEXT_FLAGS_ARB,
        WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
        0 // Terminator 2: Judgement Day
    };

    SetPixelFormat(DeviceContext, GoodFormat, &DummyPixelFormat);
    GlContext = wglCreateContextAttribsARB(DeviceContext, NULL, ContextAttributes);
    wglMakeCurrent(DeviceContext, GlContext);

    gladLoadGL(GetGlSymbol);

    LogDebug("Got %s %s context with GLSL %s on renderer %s", glGetString(GL_VENDOR), glGetString(GL_VERSION),
             glGetString(GL_SHADING_LANGUAGE_VERSION), glGetString(GL_RENDERER));

    LogDebug("Showing window");
    ShowWindow(Window, SW_SHOWDEFAULT);

    return TRUE;
}
#endif

BOOLEAN VidInitialize(_In_ BOOLEAN EnableGl)
{
    LogInfo("Initializing Windows video");

    RegisterWindowClass();
    InitializeWindow();

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

    // ImGui_ImplWin32_Init(Window);

#ifdef PURPL_OPENGL
    if (EnableGl)
    {
        return EnableOpenGl();
    }
    else
    {
#endif
        LogDebug("Showing window");
        ShowWindow(Window, SW_SHOWDEFAULT);
        return FALSE;
#ifdef PURPL_OPENGL
    }
#endif
}

BOOLEAN VidUpdate(VOID)
{
    MSG Message;

#ifdef PURPL_ENGINE
    // By the point of the first call to VidUpdate, the GPU name is known, but it isn't at the time of VidInitialize, so
    // this has to go here.
    static BOOLEAN IsFirstUpdate = TRUE;
    if (IsFirstUpdate)
    {
        PCHAR Title = CmnAppendString(WindowTitle, CmnFormatTempString(" using GPU %s", RdrGetGpuName()));
        SetWindowTextA(Window, Title); // Window proc stores this into WindowTitle
        CmnFree(Title);
        IsFirstUpdate = FALSE;
    }
#endif

    // ImGui_ImplWin32_NewFrame();

    while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
    }

#ifdef PURPL_OPENGL
    if (GlContext)
    {
        wglSwapLayerBuffers(DeviceContext, WGL_SWAP_MAIN_PLANE);
    }
#endif

    // Set in the window procedure
    return !WindowClosed;
}

VOID VidShutdown(VOID)
{
    LogInfo("Shutting down Windows video");

    // ImGui_ImplWin32_Shutdown();

#ifdef PURPL_OPENGL
    if (GlContext)
    {
        LogDebug("Destroying OpenGL context");
        wglDeleteContext(GlContext);
    }
#endif

    ReleaseDC(Window, DeviceContext);

    LogDebug("Destroying window");
    DestroyWindow(Window);

    CmnFree(WindowTitle);

    LogInfo("Successfully shut down Windows video");
}
#else
VOID VidInitialize(_In_ BOOLEAN EnableGl)
{
    UNREFERENCED_PARAMETER(EnableGl);
    WindowWidth = 1280;
    WindowHeight = 720;
}

BOOLEAN VidUpdate(VOID)
{
    return TRUE;
}

VOID VidShutdown(VOID)
{

}
#endif

VOID VidGetSize(_Out_opt_ PUINT32 Width, _Out_opt_ PUINT32 Height)
{
    Width ? *Width = WindowWidth : 0;
    Height ? *Height = WindowHeight : 0;
}

BOOLEAN VidResized(VOID)
{
    BOOLEAN ReturnValue = WindowResized;
    WindowResized = FALSE;
    return ReturnValue;
}

BOOLEAN VidFocused(VOID)
{
    return WindowFocused;
}

PVOID VidGetObject(VOID)
{
    return Window;
}

FLOAT VidGetDpi(VOID)
{
    return (FLOAT)GetDpiForWindow(Window);
}

#ifdef PURPL_VULKAN
VkSurfaceKHR VidCreateVulkanSurface(_In_ VkInstance Instance, _In_ PVOID AllocationCallbacks,
                                    _In_opt_ PVOID WindowHandle)
{
    VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {0};
    VkSurfaceKHR Surface;
    VkResult Result;

    LogDebug("Creating Vulkan surface with vkCreateWin32SurfaceKHR");

    SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    SurfaceCreateInfo.hinstance = GetModuleHandleA(NULL);
    SurfaceCreateInfo.hwnd = WindowHandle ? WindowHandle : Window;

    Surface = NULL;
    Result = vkCreateWin32SurfaceKHR(Instance, &SurfaceCreateInfo, AllocationCallbacks, &Surface);
    if (Result != VK_SUCCESS)
    {
        LogError("Failed to create Vulkan surface: VkResult %d", Result);
        return VK_NULL_HANDLE;
    }

    LogDebug("Successfully created Vulkan surface with handle 0x%llX", (UINT64)Surface);

    return Surface;
}
#endif
