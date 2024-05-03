/// @file purpl/purpl.h
///
/// @brief Global header. Changes will make a lot of stuff rebuild.
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "config.h"

#ifndef PURPL_RESOURCE_FILE

#ifdef __cplusplus
#define BEGIN_EXTERN_C                                                                                                 \
    extern "C"                                                                                                         \
    {
#define END_EXTERN_C }
#else
#define BEGIN_EXTERN_C
#define END_EXTERN_C
#endif

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#ifdef PURPL_WIN32
#if !defined(__clang__) && __STDC_VERSION__ <= 201100l
#define _Noreturn __declspec(noreturn)
#define _Thread_local __declspec(thread)
#endif

#define WIN32_LEAN_AND_MEAN
#ifdef PURPL_XBOX360
#include <xtl.h>
#include <xboxmath.h>
#else
#include <windows.h>
#include <shlobj.h>
#endif

#ifdef __cplusplus
#include <comdef.h>
#endif
#include <shellapi.h>
#include <uxtheme.h>
#include <windowsx.h>

#ifndef PURPL_GDKX
#include <dbghelp.h>
#include <psapi.h>
#include <tlhelp32.h>
#endif

#define strcasecmp _stricmp
#define fseeko64 _fseeki64
#endif

#ifdef PURPL_UNIX
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#define fseeko64 fseeko
#define stat64 stat

#if !defined PURPL_SWITCH && !defined PURPL_CONSOLE_HOMEBREW
#include <sys/syscall.h>
#include <sys/utsname.h>
#endif

#if !defined PURPL_SWITCH && !defined PURPL_CONSOLE_HOMEBREW
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>
#include <signal.h>
#endif

#include <malloc.h>
#include <unistd.h>

#include "common/wintypes.h"
#endif

#ifdef PURPL_CONSOLE_HOMEBREW
#include <pthread.h>

#define fseeko64 fseeko
#endif

#if defined PURPL_SWITCH && defined PURPL_CONSOLE_HOMEBREW
#include <switch.h>
#endif

#ifdef PURPL_PS3
#define CLOCK_MONOTONIC 4
#endif

#ifdef PURPL_PSP
#include <pspkernel.h>
#endif

#define CGLM_CLIPSPACE_INCLUDE_ALL
#include "cglm/cglm.h"

#include "cJSON.h"

#ifdef PURPL_OPENGL
#include "glad/gl.h"
#ifdef PURPL_WIN32
#include "glad/wgl.h"
#else
#include "glad/egl.h"
#endif
#endif

#ifdef PURPL_VULKAN
#include "volk.h"
#endif

#if PURPL_USE_MIMALLOC
#include "mimalloc.h"
#endif

#include "stb/stb_ds.h"
// assimp uses stb_image too
#define stbi_convert_iphone_png_to_rgb_thread stbi_convert_iphone_png_to_rgb_thread_NOCONFLICT
#define stbi_set_unpremultiply_on_load_thread stbi_set_unpremultiply_on_load_thread_NOCONFLICT
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "stb/stb_sprintf.h"

#include "xxhash.h"

#include "zstd.h"

#ifdef PURPL_ENGINE
#ifdef PURPL_DISCORD
#include "discord_rpc.h"
#endif

#include "flecs.h"
#endif


#ifdef __cplusplus
#ifndef _Noreturn
#define _Noreturn [[noreturn]]
#endif
#ifndef _Thread_local
#define _Thread_local thread_local
#endif
#endif

BEGIN_EXTERN_C

/// @brief Cross-platform entry point
///
/// @param ArgumentCount The number of command line arguments
/// @param Arguments The command line arguments
///
/// @return 0 on success, otherwise does not return
extern INT PurplMain(_In_ PCHAR *Arguments, _In_ UINT ArgumentCount);

#endif

// Switch mountpoints

#ifdef PURPL_SWITCH
#define PURPL_SWITCH_MAKE_MOUNTPOINT(Name) Name ":/"
#define PURPL_SWITCH_USERDATA_NAME "user"
#define PURPL_SWITCH_USERDATA_MOUNTPOINT PURPL_SWITCH_MAKE_MOUNTPOINT(PURPL_SWITCH_USERDATA_NAME)
#define PURPL_SWITCH_ROMFS_NAME "romfs"
#define PURPL_SWITCH_ROMFS_MOUNTPOINT PURPL_SWITCH_MAKE_MOUNTPOINT(PURPL_SWITCH_ROMFS_NAME)
#else
#define PURPL_SWITCH_USERDATA_NAME ""
#define PURPL_SWITCH_USERDATA_MOUNTPOINT ""
#define PURPL_SWITCH_ROMFS_NAME ""
#define PURPL_SWITCH_ROMFS_MOUNTPOINT ""
#endif

END_EXTERN_C
