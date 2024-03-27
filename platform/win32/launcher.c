/// @file launcher.c
///
/// @brief This file implements the Windows entry point.
///
/// @copyright (c) 2024 Randomcode Developers

#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"
#include "common/filesystem.h"

#ifndef PURPL_DEBUG
// hinting the nvidia driver to use the dedicated graphics card in an optimus
// configuration for more info, see:
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// same thing for AMD GPUs using v13.35 or newer drivers
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

// argc and argv are already supplied in debug builds, but otherwise they have
// to be parsed. These store the arguments either way.

static INT ArgumentCount;
static PCHAR *Arguments;
static BOOLEAN ParsedArguments;

extern VOID InitializeMainThread(_In_ PFN_THREAD_START StartAddress);

extern DWORD InitialConsoleInputMode;
extern DWORD InitialConsoleOutputMode;
extern DWORD InitialConsoleErrorMode;

DWORD ExceptionFilter(DWORD ExceptionCode)
{
    switch (ExceptionCode)
    {
    case EXCEPTION_BREAKPOINT:
        DebugBreak();
        return EXCEPTION_CONTINUE_EXECUTION;
    // Unless I have to, I don't really care about floating point exceptions, because why should I?
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_FLT_INEXACT_RESULT:
    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_OVERFLOW:
    case EXCEPTION_FLT_STACK_CHECK:
    case EXCEPTION_FLT_UNDERFLOW:
        LogWarning("Got floating-point exception 0x%08X at %s", ExceptionCode, PlatCaptureStackBackTrace(1, 0));
        return EXCEPTION_CONTINUE_EXECUTION;
    default:
        CmnError("Got fatal exception 0x%08X", ExceptionCode);
    }
}

/// @brief This routine is the entry point for non-debug Windows builds.
///
/// @param Instance          Module handle.
/// @param PreviousInstance  Not used.
/// @param CommandLine       Command line.
/// @param Show              Window show state.
///
/// @return An appropriate status code.
INT WINAPI WinMain(_In_ HINSTANCE Instance, _In_opt_ HINSTANCE PreviousInstance, _In_ LPSTR CommandLine, _In_ INT Show)
{
    INT Result;
    // Don't care about checking for a console parent process on Xbox, since
    // the console window won't be seen in any circumstance
#ifndef PURPL_GDKX
#ifdef PURPL_DEBUG
    DWORD Error;
#endif
    HANDLE Snapshot;
    PROCESSENTRY32 ProcessEntry = {0};
    UINT32 EngineProcessId;
    UINT32 ParentProcessId;
    HANDLE ParentProcess;
    CHAR ParentExePath[MAX_PATH] = {0};
    CHAR ParentExeDevicePath[MAX_PATH] = {0};
    CHAR DriveLetter[] = " :";
    CHAR DevicePath[MAX_PATH] = {0};
    CHAR ParentExeDosPath[MAX_PATH] = {0};
    PIMAGE_DOS_HEADER ParentDosHeader = {0};
    PIMAGE_NT_HEADERS ParentHeaders = {0};
    SIZE_T Size;
    DWORD Mode;
#endif

    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(PreviousInstance);
    UNREFERENCED_PARAMETER(Show);

    if (!Arguments)
    {
        ArgumentCount = 0;
        Arguments = CmnParseCommandline(CommandLine, &ArgumentCount);
        ParsedArguments = TRUE;
    }

#ifndef PURPL_DEBUG
    InitializeMainThread((PFN_THREAD_START)WinMain);
#endif

#if !defined PURPL_GDKX && (defined PURPL_DEBUG || defined PURPL_RELWITHDEBINFO)
    LogDebug("Attempting to load debug info");
    SymSetOptions(SYMOPT_DEBUG | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
    {
        Error = GetLastError();
        LogError("Failed to initialize DbgHelp: %d (0x%X)", Error, Error);
    }
    if (!SymLoadModuleEx(GetCurrentProcess(), NULL, PURPL_EXECUTABLE_NAME ".exe", NULL, (UINT64)GetModuleHandleA(NULL),
                         0, NULL, 0))
    {
        Error = GetLastError();
        LogError("Failed to load symbols: %d (0x%X)", Error, Error);
    }
#endif

    //__try
    //{
        Result = PurplMain(Arguments, ArgumentCount);
    //}
    //__except (ExceptionFilter(GetExceptionCode()))
    //{
    //    PEXCEPTION_POINTERS Exception = GetExceptionInformation();
    //    CmnError("Fatal exception")
    //}

    // Check if the parent process is a console, and pause if it isn't. No
    // error checking because the program is done anyway.

#ifndef PURPL_GDKX
#if _WIN32_WINNT > 0x502
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), InitialConsoleInputMode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), InitialConsoleOutputMode);
    SetConsoleMode(GetStdHandle(STD_ERROR_HANDLE), InitialConsoleErrorMode);
#endif

    Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
    ParentProcessId = 0;
    if (Process32First(Snapshot, &ProcessEntry))
    {
        EngineProcessId = GetCurrentProcessId();
        do
        {
            if (ProcessEntry.th32ProcessID == EngineProcessId)
            {
                ParentProcessId = ProcessEntry.th32ParentProcessID;
                break;
            }
        } while (Process32Next(Snapshot, &ProcessEntry));

        ParentProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ParentProcessId);
        if (ParentProcess)
        {
            GetProcessImageFileNameA(ParentProcess, ParentExePath, PURPL_ARRAYSIZE(ParentExePath));
            CloseHandle(ParentProcess);

            if (strncmp(ParentExePath, "\\Device\\", 8) == 0)
            {
                for (CHAR c = 'A'; c <= 'Z'; c++)
                {
                    strncpy(ParentExeDevicePath, ParentExePath, strchr(ParentExePath + 8, '\\') - ParentExePath);
                    DriveLetter[0] = c;
                    QueryDosDeviceA(DriveLetter, DevicePath, PURPL_ARRAYSIZE(DevicePath));
                    if (strcmp(DevicePath, ParentExeDevicePath) == 0)
                    {
                        snprintf(ParentExeDosPath, PURPL_ARRAYSIZE(ParentExeDosPath), "%c:%s", c,
                                 ParentExePath + strlen(ParentExeDevicePath));
                        break;
                    }
                }
            }

            if (!strlen(ParentExeDosPath))
            {
                strncpy(ParentExeDosPath, ParentExePath, PURPL_ARRAYSIZE(ParentExePath));
            }

            ParentDosHeader =
                (PIMAGE_DOS_HEADER)FsReadFile(TRUE, ParentExeDosPath, 0, sizeof(IMAGE_DOS_HEADER), &Size, 0);
            if (ParentDosHeader && ParentDosHeader->e_magic == IMAGE_DOS_SIGNATURE)
            {
                ParentHeaders = (PIMAGE_NT_HEADERS)FsReadFile(TRUE, ParentExeDosPath, ParentDosHeader->e_lfanew,
                                                              sizeof(IMAGE_NT_HEADERS), &Size, 0);
                if (ParentHeaders && ParentHeaders->Signature == IMAGE_NT_SIGNATURE)
                {
                    // Check bitness, offset of Subsystem is different
                    if ((ParentHeaders->FileHeader.Machine & IMAGE_FILE_32BIT_MACHINE &&
                         ((PIMAGE_NT_HEADERS32)ParentHeaders)->OptionalHeader.Subsystem !=
                             IMAGE_SUBSYSTEM_WINDOWS_CUI) ||
                        !(ParentHeaders->FileHeader.Machine & IMAGE_FILE_32BIT_MACHINE) &&
                            ((PIMAGE_NT_HEADERS64)ParentHeaders)->OptionalHeader.Subsystem !=
                                IMAGE_SUBSYSTEM_WINDOWS_CUI)
                    {
                        printf("Engine (PID %llu, parent PID %llu) returned %d.\n"
                               "Press any key to exit...",
                               (UINT64)EngineProcessId, (UINT64)ParentProcessId, Result);

                        // Disable line input so any key works and not just Enter
                        Mode = 0;
                        GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &Mode);
                        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), 0);
                        (VOID) getchar();
                        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), Mode);
                    }
                }
            }
        }
    }
#endif

    if (ParsedArguments)
        CmnFree(Arguments);

    return Result;
}

#if defined PURPL_DEBUG || defined PURPL_RELWITHDEBINFO
/// @brief This routine is the entry point for debug Windows builds.
///        It calls WinMain with the appropriate parameters.
///
/// @param[in] argc The number of command line arguments.
/// @param[in] argv The command line arguments.
///
/// @return The return value of PurplMain
INT main(_In_ INT argc, _In_ PCHAR argv[])
{
    CHAR DummyCommandline;

    Arguments = argv;
    ArgumentCount = argc;
    ParsedArguments = FALSE;

    InitializeMainThread((PFN_THREAD_START)main);

    DummyCommandline = 0;
    return WinMain(GetModuleHandleA(NULL), NULL, &DummyCommandline, SW_SHOWNORMAL);
}
#endif
