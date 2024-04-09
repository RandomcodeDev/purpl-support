/*++

Copyright (c) 2024 Randomcode Developers

Module Name:

    platform.c

Abstract:

    This file implements miscellaneous Unix-like abstractions.

--*/

#include "common/common.h"

#include "platform/platform.h"

static INT32 NxLinkSocket;

VOID PlatInitialize(VOID)
{
    
}

VOID PlatShutdown(VOID)
/*++

Routine Description:

    This routine cleans up Unix-specific resources.

Arguments:

    None.

Return Value:

    None.

--*/
{
    
}

PCSTR
PlatCaptureStackBackTrace(_In_ UINT32 FramesToSkip, _In_ UINT32 MaxFrames)
/*++

Routine Description:

    Gets a stack trace in a static buffer.

Arguments:

    FramesToSkip - The number of stack frames to skip.

    MaxFrames - The maximum number of frames to get.

Return Value:

    The address of a static buffer containing a string with
    the formatted stack trace.

--*/
{
    static CHAR Buffer[2048];

    UNREFERENCED_PARAMETER(FramesToSkip);
    UNREFERENCED_PARAMETER(MaxFrames);

#if 0
    PVOID Frames[32];
    PCHAR *Symbols;
    UINT64 Size;
    UINT64 Offset;
    UINT64 i;

    memset(Buffer, 0, PURPL_ARRAYSIZE(Buffer));

    Size = backtrace(Frames, PURPL_ARRAYSIZE(Frames));
    if (MaxFrames > 0 && Size > MaxFrames)
    {
        Size = MaxFrames + FramesToSkip;
    }
    Symbols = backtrace_symbols(Frames, Size);

    Offset = 0;
    for (i = FramesToSkip; i < Size; i++)
    {
        Offset += snprintf(Buffer + Offset, PURPL_ARRAYSIZE(Buffer) - Offset, "\t%zu: %s (0x%llx)\n", i, Symbols[i],
                           (UINT64)Frames[i]);
    }

    free(Symbols);
#endif
    return Buffer;
}

PCSTR
PlatGetDescription(VOID)
/*++

Routine Description:

    Retrieves a string with information about the system version.

Arguments:

    None.

Return Value:

    A static buffer containing the system description.

--*/
{
    static CHAR Buffer[32];

    if (!strlen(Buffer))
    {
        
    }

    return Buffer;
}

_Noreturn VOID PlatError(_In_ PCSTR Message)
{
    abort();
}

PVOID PlatGetReturnAddress(VOID)
/*++

Routine Description:

    Gets the return address.

Arguments:

    None.

Return Value:

    NULL - The return address could not be determined.

    non-NULL - The return address of the caller.

--*/
{
#ifdef PURPL_DEBUG
    return __builtin_return_address(1);
#else
    return NULL;
#endif
}

PCSTR PlatGetUserDataDirectory(VOID)
{
    static CHAR Buffer[128];

    if (!strlen(Buffer))
    {
        strncpy(Buffer, "sdmc:/" PURPL_EXECUTABLE_NAME "/", PURPL_ARRAYSIZE(Buffer));
    }

    return Buffer;
}

UINT64 PlatGetMilliseconds(VOID)
{
    struct timespec Time = {0};

    clock_gettime(CLOCK_MONOTONIC, &Time);

    return Time.tv_sec * 1000 + Time.tv_nsec / 1000000;
}

BOOLEAN PlatCreateDirectory(_In_ PCSTR Path)
{
    // https://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix

    CHAR TempPath[256];
    PCHAR p = NULL;
    UINT64 Length;

    snprintf(TempPath, sizeof(TempPath), "%s", Path);
    Length = strlen(TempPath);
    if (TempPath[Length - 1] == '/')
    {
        TempPath[Length - 1] = 0;
    }
    for (p = TempPath + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            mkdir(TempPath, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(TempPath, S_IRWXU);

    // should probably for real check return values
    return TRUE;
}

PCHAR PlatFixPath(_In_ PCSTR Path)
{
    return CmnFormatString("%s", Path);
}

UINT64 PlatGetFileSize(_In_ PCSTR Path)
{
    struct stat64 StatBuffer = {0};

    stat64(Path, &StatBuffer);
    return StatBuffer.st_size;
}
