/// @file video.c
///
/// @brief This file contains the video abstraction API.
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "purpl/purpl.h"

/// @brief Initialize platform video
///
/// @param[in] EnableGl Whether to try to enable OpenGL
///
/// @return Whether OpenGL was initialized
extern BOOLEAN VidInitialize(_In_ BOOLEAN EnableGl);

/// @brief Update video (and input)
///
/// @return Whether the window is open
extern BOOLEAN VidUpdate(VOID);

/// @brief Shut down platform video
extern VOID VidShutdown(VOID);

/// @brief Get the size of the platform video output
///
/// @param[out] Width Receives the width
/// @param[out] Height Receives the height
extern VOID VidGetSize(_Out_opt_ PUINT32 Width, _Out_opt_ PUINT32 Height);

/// @brief Get whether the video output has been resized since the last call
extern BOOLEAN VidResized(VOID);

/// @brief Get whether the video output is focused
extern BOOLEAN VidFocused(VOID);

/// @brief Get the equivalent of a window handle (HWND, GLFWwindow, etc)
///
/// @return The video output object
extern PVOID VidGetObject(VOID);

/// @brief Get the current DPI
///
/// @return The current DPI
extern FLOAT VidGetDpi(VOID);

#ifdef PURPL_VULKAN
/// @brief Get the address of a Vulkan symbol
///
/// @param[in] Instance The Vulkan instance
/// @param[in] Name The name of the symbol
///
/// @return The address of the symbol
extern PVOID PlatGetVulkanFunction(_In_ PVOID Instance, _In_z_ PCSTR Name);

/// @brief Create a Vulkan surface
///
/// @param[in] Instance The Vulkan instance
/// @param[in] AllocationCallbacks Allocation functions (should be the return
/// value of VlkGetAllocationCallbacks)
/// @param[in] WindowHandle The handle of the window to use, if not the engine's
///
/// @return A Vulkan surface
extern VkSurfaceKHR VidCreateVulkanSurface(_In_ VkInstance Instance, _In_ PVOID AllocationCallbacks,
                                           _In_opt_ PVOID WindowHandle);
#endif

/// @brief Assumed size of one pixel
#define VIDEO_FRAMEBUFFER_PIXEL_SIZE 4

/// @brief A framebuffer
typedef struct VIDEO_FRAMEBUFFER
{
    PUINT32 Pixels;
    UINT32 Width;
    UINT32 Height;
    PVOID Handle;
} VIDEO_FRAMEBUFFER, *PVIDEO_FRAMEBUFFER;

/// @brief Create a framebuffer
///
/// @return A framebuffer
extern PVIDEO_FRAMEBUFFER VidCreateFramebuffer(VOID);

/// @brief Display a framebuffer's contents, and update it if necessary
///
/// @param[in,out] Framebuffer The framebuffer to display/update
extern VOID VidDisplayFramebuffer(_Inout_ PVIDEO_FRAMEBUFFER Framebuffer);

/// @brief Destroy a framebuffer
///
/// @param[in] Framebuffer The framebuffer to destroy
extern VOID VidDestroyFramebuffer(_In_ PVIDEO_FRAMEBUFFER Framebuffer);

/// @brief Convert a pixel from 32-bit RGBA
///
/// @param[in] Pixel The color to convert
///
/// @return An equivalent value for the framebuffer
extern UINT32 VidConvertPixel(_In_ UINT32 Pixel);

#define VIDEO_PACK_COLOUR(Colour)                                                                                       \
    ((UINT8)((Colour)[0] * 255) << 24 | (UINT8)((Colour)[1] * 255) << 16 | (UINT8)((Colour)[2] * 255) << 8 |           \
     (UINT8)((Colour)[3] * 255))

#define VIDEO_UNPACK_COLOUR(Colour, ColourRaw)                                                                                  \
    {                                                                                                                  \
        Colour[0] = (UINT8)((ColourRaw >> 24) & 0xFF) / 255.0f; \
        Colour[1] = (UINT8)((ColourRaw >> 16) & 0xFF) / 255.0f; \
        Colour[2] = (UINT8)((ColourRaw >> 8) & 0xFF) / 255.0f; \
        Colour[3] = (UINT8)((ColourRaw >> 0) & 0xFF) / 255.0f; \
    }
