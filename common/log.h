/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 *
 * Modifications made by Randomcode Developers:
 * - NT code style
 * - Add option to display line number in hex (for callbacks where you get an
 * address)
 * - Add thread name to messages
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "purpl/purpl.h"

#include "platform/async.h"
#include "platform/platform.h"

#define LOG_USE_COLOR 1
#define LOG_VERSION "0.1.0"

typedef enum LOG_LEVEL
{
    LogLevelTrace,
    LogLevelDebug,
    LogLevelInfo,
    LogLevelWarning,
    LogLevelError,
    LogLevelFatal
} LOG_LEVEL;

typedef struct LOG_EVENT
{
    va_list ArgList;
    PCSTR Format;
    PCSTR File;
    struct tm *Time;
    PVOID Data;
    int64_t Line;
    BOOLEAN HexLine;
    LOG_LEVEL Level;
} LOG_EVENT;

typedef VOID (*PFN_LOG_LOG)(LOG_EVENT *Event);
typedef VOID (*PFN_LOG_LOCK)(BOOLEAN Lock, PVOID Data);

#define LogTrace(...)                                                          \
    LogMessage(LogLevelTrace, __FILE__, __LINE__, false, __VA_ARGS__)
#define LogDebug(...)                                                          \
    LogMessage(LogLevelDebug, __FILE__, __LINE__, false, __VA_ARGS__)
#define LogInfo(...)                                                           \
    LogMessage(LogLevelInfo, __FILE__, __LINE__, false, __VA_ARGS__)
#define LogWarning(...)                                                        \
    LogMessage(LogLevelWarning, __FILE__, __LINE__, false, __VA_ARGS__)
#define LogError(...)                                                          \
    LogMessage(LogLevelError, __FILE__, __LINE__, false, __VA_ARGS__)
#define LogFatal(...)                                                          \
    LogMessage(LogLevelFatal, __FILE__, __LINE__, false, __VA_ARGS__)

extern PCSTR LogGetLevelString(LOG_LEVEL Level);

extern VOID LogSetLock(PFN_LOG_LOCK Lock, PVOID Data);

extern VOID LogSetLevel(LOG_LEVEL Level);

extern LOG_LEVEL LogGetLevel(VOID);

extern VOID LogSetQuiet(BOOLEAN Quiet);

extern int LogAddCallback(PFN_LOG_LOG Callback, PVOID Data, LOG_LEVEL Level);

extern int LogAddFile(FILE *File, LOG_LEVEL Level);

extern VOID LogMessage(LOG_LEVEL Level, PCSTR File, uint64_t Line,
                       BOOLEAN HexLine, PCSTR Format, ...);

#endif
