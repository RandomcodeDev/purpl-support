/*++

Copyright (c) 2024 Randomcode Developers

Module Name:

    async.c

Abstract:

    This file implements the Unix async backend.

--*/

#include "common/alloc.h"
#include "common/common.h"

#include "platform/async.h"

_Thread_local PAS_THREAD AsCurrentThread;

static PVOID ThreadEntry(_In_ PVOID Thread)
{
    AsCurrentThread = Thread;
    AsCurrentThread->ReturnValue =
        AsCurrentThread->ThreadStart(AsCurrentThread->UserData);
    return (PVOID)(UINT64)AsCurrentThread->ReturnValue;
}

VOID InitializeMainThread(_In_ PFN_THREAD_START StartAddress)
{
    AsCurrentThread = CmnAlloc(1, sizeof(AS_THREAD));
    strncpy(AsCurrentThread->Name, "main",
            PURPL_ARRAYSIZE(AsCurrentThread->Name));
    AsCurrentThread->ThreadStart = StartAddress;
}

PAS_THREAD AsCreateThread(_In_opt_ PCSTR Name, _In_ UINT64 StackSize,
               _In_ PFN_THREAD_START ThreadStart, _In_opt_ PVOID UserData)
{
    PAS_THREAD Thread;
    INT Error;
    pthread_attr_t Attributes;

    LogInfo("Creating thread %s with %zu-byte stack, entry point 0x%llX, and "
            "userdata 0x%llX",
            Name, StackSize, ThreadStart, UserData);

    Thread = CmnAlloc(1, sizeof(AS_THREAD));
    if (!Thread)
    {
        LogError("Failed to allocate thread data: %s", strerror(errno));
        return NULL;
    }

    strncpy(Thread->Name, Name, PURPL_ARRAYSIZE(Thread->Name));
    Thread->ThreadStart = ThreadStart;
    Thread->UserData = UserData;

    Error = pthread_attr_init(&Attributes);
    if (Error != 0)
    {
        LogError("Failed to initialize thread attributes: %s", strerror(Error));
        CmnFree(Thread);
        return NULL;
    }

    Error = pthread_attr_setstacksize(&Attributes, StackSize);
    if (Error != 0)
    {
        LogError("Failed to set thread stack size: %s", strerror(Error));
        CmnFree(Thread);
        return NULL;
    }

    Error = pthread_create((pthread_t *)&Thread->Handle, &Attributes,
                           ThreadEntry, Thread);
    if (Error != 0)
    {
        LogError("Failed to create thread: %s", strerror(Error));
        CmnFree(Thread);
        return NULL;
    }

    pthread_attr_destroy(&Attributes);

    return Thread;
}

UINT_PTR AsJoinThread(_In_ PAS_THREAD Thread)
{
    PVOID ReturnValue;

    if (Thread->Handle)
    {
        ReturnValue = NULL;
        pthread_join((pthread_t)Thread->Handle, &ReturnValue);
    }

    ReturnValue = (PVOID)Thread->ReturnValue;

    CmnFree(Thread);

    return (UINT_PTR)ReturnValue;
}

VOID AsDetachThread(_In_ PAS_THREAD Thread)
{
    pthread_detach((pthread_t)Thread->Handle);
}

PAS_MUTEX AsCreateMutex(VOID)
{
    pthread_mutex_t *Mutex;

    Mutex = CmnAlloc(1, sizeof(pthread_mutex_t));
    if (!Mutex)
    {
        LogError("Failed to allocate mutex: %s", strerror(errno));
        return NULL;
    }

    pthread_mutex_t Initializer = PTHREAD_MUTEX_INITIALIZER;
    memcpy(Mutex, &Initializer, sizeof(pthread_mutex_t));

    INT32 Error = pthread_mutex_init(Mutex, NULL);
    if (Error != 0)
    {
        LogError("Failed to initialize mutex: %s", strerror(Error));
        CmnFree(Mutex);
        return NULL;
    }

    return Mutex;
}

BOOLEAN AsLockMutex(_In_ PAS_MUTEX Mutex, _In_ BOOLEAN Wait)
{
    if (Mutex)
    {
        if (Wait)
        {
            return pthread_mutex_lock(Mutex) == 0;
        }
        else
        {
            return pthread_mutex_trylock(Mutex) == 0;
        }
    }

    return FALSE;
}

VOID AsUnlockMutex(_In_ PAS_MUTEX Mutex)
{
    pthread_mutex_unlock(Mutex);
}

VOID AsDestroyMutex(_In_ PAS_MUTEX Mutex)
{
    if (Mutex)
    {
        pthread_mutex_destroy(Mutex);
        CmnFree(Mutex);
    }
}
