/*++

Copyright (c) 2024 Randomcode Developers

Module Name:

    video.c

Abstract:

    This file abstracts the Unix video functions.

--*/

#include "common/common.h"
#include "common/configvar.h"

#ifdef PURPL_ENGINE
#include "engine/render/render.h"
#endif

#include "GLFW/glfw3.h"

#include "platform/platform.h"
#include "platform/video.h"

static GLFWwindow *Window;

static PCHAR WindowTitle;

static INT WindowWidth;
static INT WindowHeight;

static BOOLEAN WindowResized;
static BOOLEAN WindowFocused;
BOOLEAN WindowClosed;
static BOOLEAN GlEnabled;

static VOID GlfwErrorCallback(_In_ INT Error, _In_ PCSTR Description)
{
    LogError("GLFW error %d: %s", Error, Description);
}

static VOID GlfwResizeCallback(_In_ GLFWwindow *ResizedWindow, _In_ INT Width, _In_ INT Height)
{
    if (ResizedWindow == Window && (Width != WindowWidth || Height != WindowHeight))
    {
        LogInfo("Window resized from %ux%u to %dx%d", WindowWidth, WindowHeight, Width, Height);
        WindowWidth = Width;
        WindowHeight = Height;
        WindowResized = TRUE;
    }
}

static VOID GlfwFocusCallback(_In_ GLFWwindow *FocusWindow, _In_ INT Focused)
{
    if (FocusWindow == Window)
    {
        WindowFocused = (BOOLEAN)Focused;
        LogInfo("Window %s focus", WindowFocused ? "gained" : "lost");
    }
}

BOOLEAN VidInitialize(_In_ BOOLEAN EnableGl)
{
    PCSTR GlfwError;

    LogInfo("Initializing Unix video using GLFW");

    if (!glfwInit())
    {
        CmnError("Failed to initialize GLFW: %d %s", glfwGetError(&GlfwError), GlfwError);
    }

    glfwSetErrorCallback(GlfwErrorCallback);

#ifdef PURPL_OPENGL
    if (EnableGl)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef PURPL_DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, TRUE);
#endif
    }
    else
    {
#endif
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#ifdef PURPL_OPENGL
    }
#endif

    WindowTitle = CmnFormatString(PURPL_NAME " | " PURPL_BUILD_TYPE " | v" PURPL_VERSION_STRING
#ifdef PURPL_DEBUG
                                             " | " PURPL_COMMIT "-" PURPL_BRANCH
#endif
#ifdef PURPL_ENGINE
                                             " | %s renderer",
                                  RdrGetApiName(CONFIGVAR_GET_INT("rdr_api"))
#endif
    );

    WindowWidth = 1280;
    WindowHeight = 720;
    LogInfo("Creating %ux%u window titled %s", WindowWidth, WindowHeight, WindowTitle);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle, NULL, NULL);
    if (!Window)
    {
        CmnError("Failed to create window: %d %s", glfwGetError(&GlfwError), GlfwError);
    }

    glfwSetWindowSizeCallback(Window, GlfwResizeCallback);
    glfwSetWindowFocusCallback(Window, GlfwFocusCallback);

    //    glfwShowWindow(Window);

#ifdef PURPL_OPENGL
    GlEnabled = EnableGl;
    if (GlEnabled)
    {
        glfwMakeContextCurrent(Window);
        gladLoadGL(glfwGetProcAddress);
    }
#endif

    return GlEnabled;
}

BOOLEAN
VidUpdate(VOID)
/*++

Routine Description:

    This routine processes window events.

Arguments:

    None.

Return Value:

    TRUE - The window is still open.

    FALSE - The window was closed.

--*/
{
    static BOOL FirstUpdate = TRUE;
    if (FirstUpdate)
    {
        FirstUpdate = FALSE;
#ifdef PURPL_ENGINE
        PCHAR NewTitle = CmnAppendString(WindowTitle, CmnFormatTempString(" using GPU %s", RdrGetGpuName()));
        CmnFree(WindowTitle);
        WindowTitle = NewTitle;
        glfwSetWindowTitle(Window, WindowTitle);
#endif
    }

    glfwPollEvents();

    if (!WindowClosed)
    {
        WindowClosed = (BOOLEAN)glfwWindowShouldClose(Window);
        if (WindowClosed)
        {
            LogDebug("Window closed");
        }
    }

#ifdef PURPL_OPENGL
    if (GlEnabled)
    {
        glfwSwapBuffers(Window);
    }
#endif

    glfwGetWindowSize(Window, &WindowWidth, &WindowHeight);

    return !WindowClosed;
}

VOID VidShutdown(VOID)
/*++

Routine Description:

    This routine cleans up video resources.

Arguments:

    None.

Return Value:

    None.

--*/
{
    LogInfo("Shutting down Unix video");

    glfwDestroyWindow(Window);
    glfwTerminate();

    LogInfo("Successfully shut down Unix video");
}

VOID VidGetSize(_Out_opt_ PUINT32 Width, _Out_opt_ PUINT32 Height)
/*++

Routine Description:

    Stores the width and/or height in the provided parameters.

Arguments:

    Width - A pointer to an integer that recieves the width of the
    window if provided.

    Height - A pointer to an integer that recieves the height of the
    window if provided.

Return Value:

    None.

--*/
{
    Width ? *Width = (UINT32)WindowWidth : 0;
    Height ? *Height = (UINT32)WindowHeight : 0;
}

BOOLEAN
VidResized(VOID)
/*++

Routine Description:

    Returns whether the window has been resized since the last call.

Arguments:

    None.

Return Value:

    The value of WindowResized.

--*/
{
    BOOLEAN ReturnValue = WindowResized;
    WindowResized = FALSE;
    return ReturnValue;
}

BOOLEAN
VidFocused(VOID)
/*++

Routine Description:

    Returns whether the window is focused.

Arguments:

    None.

Return Value:

    The value of WindowFocused.

--*/
{
    return WindowFocused;
}

FLOAT
VidGetDpi(VOID)
{
    FLOAT XDpi;
    FLOAT YDpi;

    XDpi = 0.0f;
    YDpi = 0.0f;
    glfwGetWindowContentScale(Window, &XDpi, &YDpi);

    return XDpi;
}

PVOID VidGetObject(VOID)
{
    return Window;
}

#ifdef PURPL_VULKAN
VkSurfaceKHR VidCreateVulkanSurface(_In_ VkInstance Instance, _In_ PVOID AllocationCallbacks,
                                    _In_opt_ PVOID WindowHandle)
/*++

Routine Description:

    Creates a Vulkan surface from the window.

Arguments:

    Instance - The Vulkan instance to create the surface for.

    AllocationCallbacks - The Vulkan allocation callbacks to use.

    WindowHandle - The window to create the surface for. If NULL, the engine's
window is used.

Return Value:

    NULL - Surface creation failed.

    Non-NULL - Surface creation successful.

--*/
{
    VkResult Result;
    VkSurfaceKHR Surface;

    LogDebug("Creating Vulkan surface with glfwCreateWindowSurface");

    Result = glfwCreateWindowSurface(Instance, WindowHandle ? WindowHandle : Window, AllocationCallbacks, &Surface);
    if (Result != VK_SUCCESS)
    {
        LogError("Failed to create Vulkan surface: VkResult %d", Result);
        return VK_NULL_HANDLE;
    }

    return Surface;
}
#endif
