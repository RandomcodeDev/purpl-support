/// @file async.h
///
/// @brief This file contains the async abstraction API.
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "purpl/purpl.h"

/// @brief A pointer to a thread start function
typedef UINT_PTR (*PFN_THREAD_START)(_In_opt_ PVOID UserData);

/// @brief Data about a thread
typedef struct AS_THREAD
{
    CHAR Name[32];
    PFN_THREAD_START ThreadStart;
    PVOID UserData;
    UINT_PTR ReturnValue;
    PVOID Handle;
} AS_THREAD, *PAS_THREAD;

/// @brief The current thread (not fully valid for the main thread)
extern _Thread_local PAS_THREAD AsCurrentThread;

/// @brief Default stack size for a thread
#define PURPL_DEFAULT_THREAD_STACK_SIZE 0x1000

/// @brief Create a suspended thread
///
/// @param[in] Name The name of the thread
/// @param[in] StackSize The size of the thread's stack
/// @param[in] ThreadStart The start function for the thread
/// @param[in] UserData Data for the thread
///
/// @return The new thread
extern PAS_THREAD AsCreateThread(_In_opt_ PCSTR Name, _In_ UINT64 StackSize, _In_ PFN_THREAD_START ThreadStart,
                              _In_opt_ PVOID UserData);

/// @brief Wait for a thread to finish, get its return value, and clean up the
/// THREAD structure
///
/// @param[in] Thread The thread to join
///
/// @return The thread's return value
extern UINT_PTR AsJoinThread(_In_ PAS_THREAD Thread);

/// @brief Run a thread, use AsJoinThread to clean up its data at some point
///
/// @param[in] Thread The thread to detach
extern VOID AsDetachThread(_In_ PAS_THREAD Thread);

/// @brief Suspend a thread
///
/// @param[in] Thread The thread to suspend
extern VOID AsSuspendThread(_In_ PAS_THREAD Thread);

/// @brief Resume a thread
///
/// @param[in] Thread The thread to resume
extern VOID AsResumeThread(_In_ PAS_THREAD Thread);

/// @brief A mutex
typedef PVOID PAS_MUTEX;

/// @brief Create a mutex
///
/// @return A mutex
extern PAS_MUTEX AsCreateMutex(VOID);

/// @brief Lock a mutex
///
/// @param[in] Mutex The mutex to lock
/// @param[in] Wait Whether to wait for it to be unlocked
///
/// @return Whether the mutex was locked
extern BOOLEAN AsLockMutex(_In_ PAS_MUTEX Mutex, _In_ BOOLEAN Wait);

/// @brief Unlock a mutex
///
/// @param[in] Mutex The mutex to unlock
extern VOID AsUnlockMutex(_In_ PAS_MUTEX Mutex);

/// @brief Destroy a mutex
///
/// @param[in] Mutex The mutex to destroy
/// @param[in] Wait Whether to wait before destroying the mutex
extern VOID AsDestroyMutex(_In_ PAS_MUTEX Mutex);

/// @brief A semaphore
typedef struct AS_SEMAPHORE *PAS_SEMAPHORE;

/// @brief Create a semaphore
///
/// @param[in] InitialValue The initial value of the semaphore
///
/// @return A semaphore
extern PAS_SEMAPHORE AsCreateSemaphore(_In_ UINT64 InitialValue);

/// @brief Destroy a semaphore
///
/// @param[in] Semaphore The semaphore to destroy
extern VOID AsDestroySemaphore(_Inout_ PAS_SEMAPHORE Semaphore);

/// @brief Test a semaphore
///
/// @param[in] Semaphore The semaphore to test
/// @param[in] Wait Whether to wait
///
/// @return Whether the semaphore was acquired
extern BOOLEAN AsTestSemaphore(_In_ PAS_SEMAPHORE Semaphore, _In_ BOOLEAN Wait);

/// @brief Increment a semaphore
///
/// @param[in] Semaphore The semaphore to increment
/// @param[in] Wait Whether to wait
///
/// @return Whether the semaphore was acquired
extern BOOLEAN AsIncrementSemaphore(_In_ PAS_SEMAPHORE Semaphore, _In_ BOOLEAN Wait);

/// @brief A condition variable
typedef struct AS_CONDITION_VARIABLE *PAS_CONDITION_VARIABLE;

/// @brief Create a condition variable
///
/// @return A condition variable
extern PAS_CONDITION_VARIABLE AsCreateCondition(VOID);

/// @brief Wait for a condition variable
///
/// @param[in,out] Condition The condition variable to wait for
/// @param[in] Mutex The mutex to use
extern VOID AsWaitCondition(_Inout_ PAS_CONDITION_VARIABLE Condition, _In_ PAS_MUTEX Mutex);

/// @brief Signal a condition variable
///
/// @param[in,out] Condition The condition variable to signal
extern VOID AsSignalCondition(_Inout_ PAS_CONDITION_VARIABLE Condition);

/// @brief Broadcast a condition variable
///
/// @param[in,out] Condition The condition variable to broadcast
extern VOID AsBroadcastCondition(_Inout_ PAS_CONDITION_VARIABLE Condition);
