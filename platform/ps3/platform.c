#include "common/common.h"

#include "platform/platform.h"

// According to the GTA 5 source code, PS3 exception handling is a pain in the ass, so I'm not gonna do it (I'd have to copy leaked code and adapt it to the homebrew SDK anyway, and the second part seems annoying).

VOID PlatInitialize(VOID)
{
}

VOID PlatShutdown(VOID)
{
}

PCSTR PlatCaptureStackBackTrace(_In_ UINT32 FramesToSkip, _In_ UINT32 MaxFrames)
{
    // TODO: PS3 backtraces
}

PCSTR PlatGetDescription(VOID)
{
    static CHAR Buffer[128];
    
    if (!strlen(Buffer))
    {
        // TODO: PS3 system info
        strncpy(Buffer, "PlayStation 3 GameOS", PURPL_ARRAYSIZE(Buffer));
    }

    return Buffer;
}

_Noreturn VOID PlatError(_In_ PCSTR Message)
{
    // TODO: PS3 error message
}

PVOID PlatGetReturnAddress(VOID)
{
#ifdef PURPL_DEBUG
    return __builtin_return_address(1);
#else
    return NULL;
#endif
}

PCSTR PlatGetUserDataDirectory(VOID)
{
    // TODO: PS3 user data (saves basically)
    return "";
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
    return CmnDuplicateString(Path, 0);
}

UINT64 PlatGetFileSize(_In_ PCSTR Path)
{
    struct stat64 StatBuffer = {0};

    stat64(Path, &StatBuffer);
    return StatBuffer.st_size;
}

