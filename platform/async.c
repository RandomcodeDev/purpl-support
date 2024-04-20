#include "common/alloc.h"

#include "async.h"

// https://cseweb.ucsd.edu/classes/sp17/cse120-a/applications/ln/lecture7.html

typedef struct SEMAPHORE
{
    UINT64 Value;
    PMUTEX Mutex;
} SEMAPHORE, *PSEMAPHORE;

PSEMAPHORE AsCreateSemaphore(_In_ UINT64 InitialValue)
{
    PSEMAPHORE Semaphore = CmnAlloc(1, sizeof(Semaphore));
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

VOID AsDestroySemaphore(_Inout_ PSEMAPHORE Semaphore)
{
    if (Semaphore)
    {
        AsDestroyMutex(Semaphore->Mutex);
        CmnFree(Semaphore);
    }
}


