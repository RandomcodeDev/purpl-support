/// @file common.c
///
/// @brief This file implements assorted common functions.
///
/// @copyright (c) 2024 Randomcode Developers

#include "common.h"
#include "alloc.h"
#include "configvar.h"
#include "filesystem.h"

static VOID LogLock(BOOLEAN Lock, PVOID Mutex)
{
    Lock ? (VOID)AsLockMutex(Mutex, TRUE) : AsUnlockMutex(Mutex);
}

static VOID ParseVariables(_In_ PCHAR *Arguments, _In_ UINT ArgumentCount)
{
    for (UINT i = 1; i < ArgumentCount; i++)
    {
        // Check if the first character is in the list of prefixes
        if (strchr(PURPL_ARGUMENT_PREFIXES, Arguments[i][0]))
        {
            PCHAR VariableName = CmnFormatTempString("%s", Arguments[i] + 1);
            PCSTR Value = "1";
            if (i < ArgumentCount - 1)
            {
                Value = Arguments[++i];
            }

            PCONFIGVAR Variable = CfgGetVariable(VariableName);
            if (Variable)
            {
                if (Variable->Internal)
                {
                    LogWarning("Ignoring internal variable %s specified on command line", VariableName);
                    continue;
                }

                LogInfo("Setting variable %s to %s from command line", VariableName, Value);
                switch (Variable->Type)
                {
                case ConfigVarTypeBoolean:
                    if (tolower(Value[0]) == 't' || tolower(Value[0]) == 'y' ||
                        strtoll(Value, NULL, 10) > 0) // true or yes
                    {
                        Variable->Current.Boolean = TRUE;
                    }
                    else
                    {
                        Variable->Current.Boolean = FALSE;
                    }
                    break;
                case ConfigVarTypeInteger:
                    Variable->Current.Int = strtoll(Value, NULL, 10);
                    break;
                case ConfigVarTypeFloat:
                    Variable->Current.Float = strtod(Value, NULL);
                    break;
                case ConfigVarTypeString:
                    strncpy(Variable->Current.String, Value, PURPL_ARRAYSIZE(Variable->Current.String));
                    break;
                }
            }
        }
    }
}

static PMUTEX LogMutex;

VOID CmnInitialize(_In_opt_ PCHAR *Arguments, _In_opt_ UINT ArgumentCount)
{
    LOG_LEVEL Level;

    PlatInitialize();

    CONST UINT8 HugePageCount = 2;
#if PURPL_USE_MIMALLOC
    LogInfo("Using mimalloc allocator");

    mi_option_set(mi_option_reserve_huge_os_pages, HugePageCount);
    mi_option_set(mi_option_show_errors, TRUE);
#else
    LogInfo("Using libc allocator");

    // Reserve 2 GiB of memory, just like mimalloc
    free(malloc(HugePageCount * 1024 * 1024 * 1024));
#endif

    LogMutex = AsCreateMutex();
    if (!LogMutex)
    {
        CmnError("Failed to create log mutex");
    }
    LogSetLock(LogLock, LogMutex);

    CONFIGVAR_DEFINE_BOOLEAN("verbose", FALSE, TRUE, ConfigVarSideBoth, FALSE, FALSE);

    if (ArgumentCount && Arguments)
    {
        ParseVariables(Arguments, ArgumentCount);
    }

#ifdef PURPL_DEBUG
    if (CONFIGVAR_GET_BOOLEAN("verbose"))
    {
        Level = LogLevelTrace;
#if PURPL_USE_MIMALLOC
        mi_option_set(mi_option_verbose, TRUE);
#endif
    }
    else
    {
        Level = LogLevelDebug;
    }
#elif defined PURPL_RELEASE
    Level = LogLevelInfo;
#endif
    LogSetLevel(Level);

    LogInfo("Common library initialized");
}

#if PURPL_USE_MIMALLOC
static VOID MiMallocStatPrint(PCSTR Message, PVOID Argument)
{
    UNREFERENCED_PARAMETER(Argument);
    // Don't want blanks between log messages, looks bad
    LogDebug("%.*s", strlen(Message) - (Message[strlen(Message) - 2] == '\n' ? 2 : 1), Message);
}
#endif

// Have to actually know the size for hashmaps
PURPL_MAKE_HASHMAP_ENTRY(CONFIGVAR_MAP, PCHAR, struct CONFIGVAR *);
extern PCONFIGVAR_MAP CfgVariables;

extern struct FILESYSTEM_SOURCE *FsSources;

VOID CmnShutdown(VOID)
{
    if (CfgVariables)
    {
        stbds_shfree(CfgVariables);
    }

    if (FsSources)
    {
        stbds_arrfree(FsSources);
    }

    PlatShutdown();

    AsDestroyMutex(LogMutex);

#if PURPL_USE_MIMALLOC
    // Some memory will still be in use because of the THREAD for the main
    // thread, which is managed by the launcher, and therefore can't be freed
    // before this function
    mi_stats_print_out(MiMallocStatPrint, NULL);
#endif

    LogInfo("Common library shut down");
}

PCSTR CmnFormatTempString(_In_z_ _Printf_format_string_ PCSTR Format, ...)
{
    va_list Arguments;
    PCSTR Formatted;

    va_start(Arguments, Format);
    Formatted = CmnFormatTempStringVarArgs(Format, Arguments);
    va_end(Arguments);

    return Formatted;
}

PCSTR CmnFormatTempStringVarArgs(_In_z_ _Printf_format_string_ PCSTR Format, _In_ va_list Arguments)
{
    static CHAR Buffer[1024];
    va_list _Arguments;

    memset(Buffer, 0, PURPL_ARRAYSIZE(Buffer));

    va_copy(_Arguments, Arguments);
    vsnprintf(Buffer, PURPL_ARRAYSIZE(Buffer), Format, _Arguments);
    va_end(_Arguments);

    return Buffer;
}

PCHAR CmnFormatStringVarArgs(_In_z_ _Printf_format_string_ PCSTR Format, _In_ va_list Arguments)
{
    PCHAR Buffer;
    INT Size;
    va_list CopiedArguments;

    va_copy(CopiedArguments, Arguments);
    Size = vsnprintf(NULL, 0, Format, CopiedArguments) + 1;
    Buffer = CmnAlloc(Size, 1);
    va_copy(CopiedArguments, Arguments);
    vsnprintf(Buffer, Size, Format, CopiedArguments);
    va_end(CopiedArguments);

    return Buffer;
}

PCHAR CmnFormatString(_In_z_ _Printf_format_string_ PCSTR Format, ...)
{
    va_list Arguments;
    PCHAR Formatted;

    va_start(Arguments, Format);
    Formatted = CmnFormatStringVarArgs(Format, Arguments);
    va_end(Arguments);

    return Formatted;
}

PCSTR CmnFormatSize(_In_ DOUBLE Size)
{
    static CHAR Buffer[64]; // Not gonna be bigger than this
    DOUBLE Value;
    UINT8 Prefix;

    static CONST PCSTR Units[] = {"B", "kiB", "MiB", "GiB", "TiB", "PiB (damn)", "EiB (are you sure?)",
                                  // NOTE: these don't all go in increments of 1024, but they're physically
                                  // impossible and here as a joke anyway
                                  "ZiB (who are you?)", "YiB (what are you doing?)", "RiB (why are you doing this?)",
                                  "QiB (HOW ARE YOU DOING THIS?)", "?B (what did you do?)"};

    Value = Size;
    Prefix = 0;
    while (Value >= 1024)
    {
        Value /= 1024;
        Prefix++;
    }

    // If close enough to 2 places of pi, use the character
    if (fabs(Value - GLM_PI) < 1e-2)
    {
        snprintf(Buffer, PURPL_ARRAYSIZE(Buffer), "π %s", Units[PURPL_MIN(Prefix, PURPL_ARRAYSIZE(Units) - 1)]);
    }
    else
    {
        snprintf(Buffer, PURPL_ARRAYSIZE(Buffer), "%.02lf %s", Value,
                 Units[PURPL_MIN(Prefix, PURPL_ARRAYSIZE(Units) - 1)]);
    }

    return Buffer;
}

PCHAR CmnInsertString(_In_z_ PCSTR String, _In_z_ PCSTR New, _In_ SIZE_T Index)
{
    if (!String || !New)
    {
        return NULL;
    }

    SIZE_T Size = strlen(String) + strlen(New) + 1;
    PCHAR NewString = CmnAlloc(Size, 1);
    if (!NewString)
    {
        return NULL;
    }

    if (Index > 1)
    {
        strncat(NewString, String, PURPL_MIN(Size, Index));
    }

    strncat(NewString, New, Size);

    if (Index < strlen(String))
    {
        strncat(NewString, String + Index, Size);
    }

    return NewString;
}

PCHAR CmnAppendString(_In_z_ PCSTR String, _In_z_ PCSTR New)
{
    return CmnInsertString(String, New, SIZE_MAX);
}

PCHAR CmnDuplicateString(_In_z_ PCSTR String, _In_ SIZE_T Count)
{
    if (!String)
    {
        return NULL;
    }

    if (Count == 0 || Count > strlen(String))
    {
        Count = strlen(String);
    }

    PCHAR New = CmnAlloc(Count + 1, sizeof(CHAR));
    if (!New)
    {
        return NULL;
    }

    strncpy(New, String, Count);

    return New;
}

_Noreturn VOID CmnErrorEx(_In_ BOOLEAN ShutdownFirst, _In_z_ _Printf_format_string_ PCSTR Message, ...)
{
    va_list Arguments;
    PCSTR FormattedMessage;
    PCSTR Formatted;
    PCSTR BackTrace;

    if (ShutdownFirst)
    {
        CmnShutdown();
    }

    va_start(Arguments, Message);
    FormattedMessage = CmnFormatStringVarArgs(Message, Arguments);
    va_end(Arguments);
#ifdef PURPL_DEBUG
    UINT32 MaxFrames = 5;
#else
    UINT32 MaxFrames = 3;
#endif
    if (CONFIGVAR_GET_BOOLEAN("verbose"))
    {
        MaxFrames = 0;
    }
    BackTrace = PlatCaptureStackBackTrace(1, // Don't include CmnError in the trace
                                          MaxFrames);
    Formatted = CmnFormatString("Fatal error: %s\nStack trace:\n%s", FormattedMessage, BackTrace);
    LogFatal("%s", Formatted);
    PlatError(Formatted);
}

// note: this is GPL code from ReactOS, copyright is theirs
PSTR *CmnParseCommandline(_In_ PCSTR lpCmdline, _Out_ PUINT32 numargs)
{
    DWORD argc;
    LPSTR *argv;
    LPCSTR s;
    LPSTR d;
    LPSTR cmdline;
    int qcount, bcount;

    if (!lpCmdline || !numargs)
    {
        return NULL;
    }

    /* --- First count the arguments */
    argc = 1;
    s = lpCmdline;
    /* The first argument, the executable path, follows special rules */
    if (*s == '"')
    {
        /* The executable path ends at the next quote, no matter what */
        s++;
        while (*s)
            if (*s++ == '"')
                break;
    }
    else
    {
        /* The executable path ends at the next space, no matter what */
        while (*s && *s != ' ' && *s != '\t')
            s++;
    }
    /* skip to the first argument, if any */
    while (*s == ' ' || *s == '\t')
        s++;
    if (*s)
        argc++;

    /* Analyze the remaining arguments */
    qcount = bcount = 0;
    while (*s)
    {
        if ((*s == ' ' || *s == '\t') && qcount == 0)
        {
            /* skip to the next argument and count it if any */
            while (*s == ' ' || *s == '\t')
                s++;
            if (*s)
                argc++;
            bcount = 0;
        }
        else if (*s == '\\')
        {
            /* '\', count them */
            bcount++;
            s++;
        }
        else if (*s == '"')
        {
            /* '"' */
            if ((bcount & 1) == 0)
                qcount++; /* unescaped '"' */
            s++;
            bcount = 0;
            /* consecutive quotes, see comment in copying code below */
            while (*s == '"')
            {
                qcount++;
                s++;
            }
            qcount = qcount % 3;
            if (qcount == 2)
                qcount = 0;
        }
        else
        {
            /* a regular character */
            bcount = 0;
            s++;
        }
    }

    /* Allocate in a single lump, the string array, and the strings that go
     * with it. This way the caller can make a single LocalFree() call to free
     * both, as per MSDN.
     */
    argv = CmnAlloc((argc + 1) * sizeof(LPSTR) + (strlen(lpCmdline) + 1) * sizeof(CHAR), 1);
    if (!argv)
        return NULL;
    cmdline = (LPSTR)(argv + argc + 1);
    strcpy(cmdline, lpCmdline);

    /* --- Then split and copy the arguments */
    argv[0] = d = cmdline;
    argc = 1;
    /* The first argument, the executable path, follows special rules */
    if (*d == '"')
    {
        /* The executable path ends at the next quote, no matter what */
        s = d + 1;
        while (*s)
        {
            if (*s == '"')
            {
                s++;
                break;
            }
            *d++ = *s++;
        }
    }
    else
    {
        /* The executable path ends at the next space, no matter what */
        while (*d && *d != ' ' && *d != '\t')
            d++;
        s = d;
        if (*s)
            s++;
    }
    /* close the executable path */
    *d++ = 0;
    /* skip to the first argument and initialize it if any */
    while (*s == ' ' || *s == '\t')
        s++;
    if (!*s)
    {
        /* There are no parameters so we are all done */
        argv[argc] = NULL;
        *numargs = argc;
        return argv;
    }

    /* Split and copy the remaining arguments */
    argv[argc++] = d;
    qcount = bcount = 0;
    while (*s)
    {
        if ((*s == ' ' || *s == '\t') && qcount == 0)
        {
            /* close the argument */
            *d++ = 0;
            bcount = 0;

            /* skip to the next one and initialize it if any */
            do
            {
                s++;
            } while (*s == ' ' || *s == '\t');
            if (*s)
                argv[argc++] = d;
        }
        else if (*s == '\\')
        {
            *d++ = *s++;
            bcount++;
        }
        else if (*s == '"')
        {
            if ((bcount & 1) == 0)
            {
                /* Preceded by an even number of '\', this is half that
                 * number of '\', plus a quote which we erase.
                 */
                d -= bcount / 2;
                qcount++;
            }
            else
            {
                /* Preceded by an odd number of '\', this is half that
                 * number of '\' followed by a '"'
                 */
                d = d - bcount / 2 - 1;
                *d++ = '"';
            }
            s++;
            bcount = 0;
            /* Now count the number of consecutive quotes. Note that qcount
             * already takes into account the opening quote if any, as well as
             * the quote that lead us here.
             */
            while (*s == '"')
            {
                if (++qcount == 3)
                {
                    *d++ = '"';
                    qcount = 0;
                }
                s++;
            }
            if (qcount == 2)
                qcount = 0;
        }
        else
        {
            /* a regular character */
            *d++ = *s++;
            bcount = 0;
        }
    }
    *d = '\0';
    argv[argc] = NULL;
    *numargs = argc;

    return argv;
}
