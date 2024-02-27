/*++

Copyright (c) 2024 Randomcode Developers

Module Name:

    video.c

Abstract:

    This file abstracts the Unix video functions.

--*/

#include "common/common.h"

#undef eglGetProcAddress
extern GLADapiproc eglGetProcAddress(PCSTR Name);
#undef eglGetDisplay
extern EGLDisplay* eglGetDisplay(EGLNativeDisplayType Display);
#undef eglInitialize
extern EGLBoolean eglInitialize(EGLDisplay dpy, EGLint * major, EGLint * minor);
#undef eglBindAPI
extern EGLBoolean eglBindAPI(EGLenum Api);

#include "platform/platform.h"
#include "platform/video.h"

static NWindow *Window;

static EGLDisplay Display;
static EGLContext GlContext;
static EGLSurface Surface;

static UINT32 WindowWidth;
static UINT32 WindowHeight;

static BOOLEAN WindowResized;

#ifdef PURPL_OPENGL
static VOID InitializeEgl(VOID)
{
    // Pretty much the same as WGL

    Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!Display)
    {
        CmnError("Failed to get default EGL display: %s", eglGetError());
    }

    gladLoadEGL(Display, eglGetProcAddress);

    eglInitialize(Display, NULL, NULL);

    if (!eglBindAPI(EGL_OPENGL_API))
    {
        CmnError("Failed to set EGL API to OpenGL: %s", eglGetError());
    }

    static CONST INT32 FramebufferAttributes[] = {EGL_RENDERABLE_TYPE,
                                                  EGL_OPENGL_BIT,
                                                  EGL_RED_SIZE,
                                                  8,
                                                  EGL_BLUE_SIZE,
                                                  8,
                                                  EGL_GREEN_SIZE,
                                                  8,
                                                  EGL_ALPHA_SIZE,
                                                  8,
                                                  EGL_DEPTH_SIZE,
                                                  24,
                                                  EGL_STENCIL_SIZE,
                                                  8,
                                                  EGL_NONE};

    EGLConfig Config = NULL;
    INT32 ConfigCount = 0;
    eglChooseConfig(Display, FramebufferAttributes, &Config, 1, &ConfigCount);
    if (ConfigCount < 1)
    {
        CmnError("No framebuffer configuration found: %s", eglGetError());
    }

    Surface = eglCreateWindowSurface(Display, Config, (EGLNativeWindowType)Window, NULL);
    if (!Surface)
    {
        CmnError("Failed to create window surface: %s", eglGetError());
    }

    static CONST INT32 ContextAttributes[] = {EGL_CONTEXT_OPENGL_PROFILE_MASK,
                                              EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
                                              EGL_CONTEXT_MAJOR_VERSION,
                                              4,
                                              EGL_CONTEXT_MAJOR_VERSION,
                                              6,
                                              EGL_NONE};
    GlContext = eglCreateContext(Display, Config, EGL_NO_CONTEXT, ContextAttributes);
    if (!GlContext)
    {
        CmnError("Failed to create OpenGL context: %s", eglGetError());
    }

    eglMakeCurrent(Display, Surface, Surface, GlContext);

    gladLoadGL(eglGetProcAddress);
}
#endif

BOOLEAN VidInitialize(_In_ BOOLEAN EnableGl)
{
    LogInfo("Initializing Switch homebrew video");

    Window = nwindowGetDefault();
    nwindowSetDimensions(Window, 1920, 1080);
    nwindowGetDimensions(Window, &WindowWidth, &WindowHeight);
    WindowResized = FALSE;

#ifdef PURPL_OPENGL
    if (EnableGl)
    {
        InitializeEgl();
        return TRUE;
    }
#endif
    return FALSE;
}

#define HANDHELD_WIDTH 1280
#define HANDHELD_HEIGHT 720
#define DOCKED_WIDTH 1920
#define DOCKED_HEIGHT 1080

BOOLEAN VidUpdate(VOID)
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
    switch (appletGetOperationMode())
    {
    case AppletOperationMode_Handheld:
        if (WindowWidth != HANDHELD_WIDTH || WindowHeight != HANDHELD_HEIGHT)
        {
            WindowWidth = HANDHELD_WIDTH;
            WindowHeight = HANDHELD_HEIGHT;
            WindowResized = TRUE;
            LogInfo("Switched to handheld mode");
        }
        break;
    case AppletOperationMode_Console:
        if (WindowWidth != DOCKED_WIDTH || WindowHeight != DOCKED_HEIGHT)
        {
            WindowWidth = DOCKED_WIDTH;
            WindowHeight = DOCKED_HEIGHT;
            WindowResized = TRUE;
            LogInfo("Switched to docked mode");
        }
        break;
    }

#ifdef PURPL_OPENGL
    eglSwapBuffers(Display, Surface);
#endif

    return appletMainLoop();
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
    LogInfo("Shutting down Switch homebrew video");

#ifdef PURPL_OPENGL
    if (GlContext)
    {
        LogInfo("Destroying OpenGL context");
        eglDestroyContext(Display, GlContext);
        GlContext = NULL;
    }

    if (Surface)
    {
        LogInfo("Destroying EGL surface");
        eglDestroySurface(Display, Surface);
        Surface = NULL;
    }

    eglTerminate(Display);
    Display = NULL;
#endif

    LogInfo("Successfully shut down Switch homebrew video");
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
    return FALSE;
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
    return TRUE;
}

FLOAT
VidGetDpi(VOID)
{
    return 1.0;
}
