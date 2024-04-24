/*++

Copyright (c) 2024 Randomcode Developers

Module Name:

    async.c

Abstract:

    This file implements the Windows async backend.

--*/

#include "common/alloc.h"
#include "common/common.h"

#include "platform/async.h"

_Thread_local PAS_THREAD AsCurrentThread;

static VOID ThreadEntry(_In_ PVOID Thread)
{
    AsCurrentThread = Thread;
    AsCurrentThread->ReturnValue = AsCurrentThread->ThreadStart(AsCurrentThread->UserData);
    ExitThread((DWORD)AsCurrentThread->ReturnValue);
}

VOID InitializeMainThread(_In_ PFN_THREAD_START StartAddress)
{
    AsCurrentThread = CmnAllocType(1, AS_THREAD);
    if (AsCurrentThread)
    {
        strncpy(AsCurrentThread->Name, "main", PURPL_ARRAYSIZE(AsCurrentThread->Name));
        AsCurrentThread->ThreadStart = StartAddress;
    }
}

PAS_THREAD AsCreateThread(_In_opt_ PCSTR Name, _In_ UINT64 StackSize, _In_ PFN_THREAD_START ThreadStart, _In_opt_ PVOID UserData)
{
    PAS_THREAD Thread;
    DWORD Error;

    LogInfo("Creating thread %s with %zu-byte stack, entry point 0x%llX, and "
            "userdata 0x%llX",
            Name, StackSize, ThreadStart, UserData);

    Thread = CmnAllocType(1, AS_THREAD);
    if (!Thread)
    {
        LogError("Failed to allocate thread data: %s", strerror(errno));
        return NULL;
    }

    strncpy(Thread->Name, Name, PURPL_ARRAYSIZE(Thread->Name));
    Thread->ThreadStart = ThreadStart;
    Thread->UserData = UserData;

    Thread->Handle = CreateThread(NULL, StackSize, (LPTHREAD_START_ROUTINE)ThreadEntry, Thread, CREATE_SUSPENDED, NULL);
    if (!Thread->Handle)
    {
        Error = GetLastError();
        LogError("Failed to create thread: %d (0x%X)", Error, Error);
        CmnFree(Thread);
        return NULL;
    }

    return Thread;
}

UINT_PTR AsJoinThread(_In_ PAS_THREAD Thread)
{
    UINT_PTR ReturnValue;

    if (Thread->Handle)
    {
        WaitForSingleObject(Thread->Handle, INFINITE);
    }

    ReturnValue = Thread->ReturnValue;

    CmnFree(Thread);

    return ReturnValue;
}

VOID AsDetachThread(_In_ PAS_THREAD Thread)
{
    AsResumeThread(Thread);
}

VOID AsSuspendThread(_In_ PAS_THREAD Thread)
{
    SuspendThread(Thread->Handle);
}

VOID AsResumeThread(_In_ PAS_THREAD Thread)
{
    ResumeThread(Thread->Handle);
}

PAS_MUTEX AsCreateMutex(VOID)
{
    return CreateMutexA(NULL, FALSE, NULL);
}

BOOLEAN AsLockMutex(_In_ PAS_MUTEX Mutex, _In_ BOOLEAN Wait)
{
    return WaitForSingleObjectEx(Mutex, Wait ? INFINITE : 0, FALSE) == WAIT_OBJECT_0;
}

VOID AsUnlockMutex(_In_ PAS_MUTEX Mutex)
{
    ReleaseMutex(Mutex);
}

VOID AsDestroyMutex(_In_ PAS_MUTEX Mutex)
{
    CloseHandle(Mutex);
}
