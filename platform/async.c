#include "common/alloc.h"

#include "async.h"

// I don't fully understand condition variables and I don't think Vulkan's semaphores are exactly the same as these, but
// this should be good enough for flecs
// https://cseweb.ucsd.edu/classes/sp17/cse120-a/applications/ln/lecture7.html

typedef struct AS_SEMAPHORE
{
    UINT64 Value;
    PAS_MUTEX Mutex;
} SEMAPHORE, *PAS_SEMAPHORE;

PAS_SEMAPHORE AsCreateSemaphore(_In_ UINT64 InitialValue)
{
    PAS_SEMAPHORE Semaphore = CmnAllocType(1, SEMAPHORE);
    if (!Semaphore)
    {
        return NULL;
    }

    Semaphore->Value = InitialValue;
    Semaphore->Mutex = AsCreateMutex();
    if (!Semaphore->Mutex)
    {
        CmnFree(Semaphore);
        return NULL;
    }

    return Semaphore;
}

VOID AsDestroySemaphore(_Inout_ PAS_SEMAPHORE Semaphore)
{
    if (Semaphore)
    {
        AsDestroyMutex(Semaphore->Mutex);
        CmnFree(Semaphore);
    }
}

BOOLEAN AsTestSemaphore(_In_ PAS_SEMAPHORE Semaphore, _In_ BOOLEAN Wait)
{
    do
    {
        if (AsLockMutex(Semaphore->Mutex, Wait))
        {
            if (Semaphore->Value <= 0)
            {
                AsUnlockMutex(Semaphore->Mutex);
                continue;
            }
            else
            {
                Semaphore->Value--;
                AsUnlockMutex(Semaphore->Mutex);
                break;
            }
        }
        else
        {
            return FALSE;
        }
    } while (Wait);
}

BOOLEAN AsIncrementSemaphore(_In_ PAS_SEMAPHORE Semaphore, _In_ BOOLEAN Wait)
{
    if (AsLockMutex(Semaphore->Mutex, Wait))
    {
        Semaphore->Value++;
        AsUnlockMutex(Semaphore->Mutex);
    }
}

typedef struct AS_CONDITION_VARIABLE
{
    PAS_MUTEX Mutex;
    PAS_THREAD Next;
    PAS_THREAD Previous;
} CONDITION_VARIABLE, *PAS_CONDITION_VARIABLE;

PAS_CONDITION_VARIABLE AsCreateCondition(VOID)
{
    PAS_CONDITION_VARIABLE Condition = CmnAllocType(1, CONDITION_VARIABLE);
}

VOID AsWaitCondition(_Inout_ PAS_CONDITION_VARIABLE Condition, _In_ PAS_MUTEX Mutex)
{
}

VOID AsSignalCondition(_In_ PAS_CONDITION_VARIABLE Condition)
{
}

VOID AsBroadcastCondition(_In_ PAS_CONDITION_VARIABLE Condition)
{
}
