/// @file common.h
///
/// @brief This file contains definitions for the common library.
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "purpl/purpl.h"

#include "platform/platform.h"

#include "log.h"

/// @brief Quit if a condition is false
#define PURPL_ASSERT(Condition)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(Condition))                                                                                              \
            CmnError("Assertion failed: " #Condition);                                                                 \
    } while (0);

/// @brief Size of an array
#define PURPL_ARRAYSIZE(Array) (sizeof(Array) / sizeof((Array)[0]))

/// @brief Larger of two values
#define PURPL_MAX(A, B) ((A) > (B) ? (A) : (B))

/// @brief Smaller of two values
#define PURPL_MIN(A, B) ((A) < (B) ? (A) : (B))

/// @brief Clamp a value to a range
#define PURPL_CLAMP(Value, Min, Max) ((Value) > (Max) ? (Max) : (Value) < (Min) ? (Min) : (Value))

/// @brief Make a string
#define PURPL_STRINGIZE(X) #X

/// @brief Make a string, expanding one level of macros
#define PURPL_STRINGIZE_EXPAND(X) PURPL_STRINGIZE(X)

/// @brief Create a struct/enum/union with the right name format
#define PURPL_MAKE_TAG(type_, name, ...)                                                                               \
    typedef type_ name __VA_ARGS__ name, *P##name;                                                                     \
    typedef type_ name const *PC##name;

/// @brief Make an ECS component
#define PURPL_MAKE_COMPONENT(type_, name, ...)                                                                         \
    PURPL_MAKE_TAG(type_, name, __VA_ARGS__) extern ECS_COMPONENT_DECLARE(name);

/// @brief Define a hashmap entry structure for stb
#define PURPL_MAKE_HASHMAP_ENTRY(Name, Tk, Tv)                                                                         \
    PURPL_MAKE_TAG(struct, Name, {                                                                                     \
        Tk key;                                                                                                        \
        Tv value;                                                                                                      \
    })

/// @brief Define a string hashmap entry structure for stb
#define PURPL_MAKE_STRING_HASHMAP_ENTRY(Name, Tv) PURPL_MAKE_HASHMAP_ENTRY(Name, PCHAR, Tv)

/// @brief Round a size to an aligment
///
/// @param[in] Alignment The alignment
/// @param[in] Size The unrounded size
///
/// @return The size rounded to be aligned to the alignment
#define PURPL_ALIGN(Alignment, Size) ((((Size) / (Alignment)) + 1) * (Alignment))

/// @brief Swap two variables
#define PURPL_SWAP(Type, A, B)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        Type Swap_ = A;                                                                                                \
        A = B;                                                                                                         \
        B = Swap_;                                                                                                         \
    } while (0)

/// @brief Characters a command line argument can start with
#define PURPL_ARGUMENT_PREFIXES "-/"

/// @brief Initialize the common library
///
/// @param[in] Arguments The command line arguments from PurplMain
/// @param[in] ArgumentCount The number of command line arguments
extern VOID CmnInitialize(_In_opt_ PCHAR *Arguments, _In_opt_ UINT ArgumentCount);

/// @brief Shut down the common library
extern VOID CmnShutdown(VOID);

/// @brief This routine formats a printf format string into a static
///        buffer for temporary usage.
///
/// @param[in] Format     The format string. You're making a bad decision if this
///                   parameter is not a string literal.
/// @param[in] Arguments  Arguments to the format string.
///
/// @return A pointer to a static buffer with the formatted string.
extern PCSTR CmnFormatTempString(_In_z_ _Printf_format_string_ PCSTR Format, ...);

/// @brief This routine formats a printf format string into a static
///        buffer for temporary usage.
///
/// @param[in] Format  The format string. You're making a bad decision if this
///                parameter is not a string literal.
/// @param[in] ...     Arguments to the format string.
///
/// @return A pointer to a static buffer with the formatted string.
extern PCSTR CmnFormatTempStringVarArgs(_In_z_ _Printf_format_string_ PCSTR Format, _In_ va_list Arguments);

/// @brief This routine formats a printf format string into a dynamically
/// allocated
///        buffer.
///
/// @param[in] Format     The format string. You're making a bad decision if this
///                   parameter is not a string literal.
/// @param[in] Arguments  Arguments to the format string.
///
/// @return A pointer to a buffer with the formatted string and a NUL terminator
///         (size is strlen(Buffer) + 1).
extern PCHAR CmnFormatString(_In_z_ _Printf_format_string_ PCSTR Format, ...);

/// @brief This routine formats a printf format string into a dynamically
/// allocated
///        buffer.
///
/// @param[in] Format  The format string. You're making a bad decision if this
///                parameter is not a string literal.
/// @param[in] ...     Arguments to the format string.
///
/// @return A pointer to a buffer with the formatted string.
extern PCHAR CmnFormatStringVarArgs(_In_z_ _Printf_format_string_ PCSTR Format, _In_ va_list Arguments);

/// @brief This routine converts a size into a human-readable string, using the
///        most appropriate unit.
///
/// @param[in] Size The size to convert.
///
/// @return The address of a static buffer containing the string.
extern PCSTR CmnFormatSize(_In_ DOUBLE Size);

/// @brief Insert a string in a string
///
/// @param[in] String The string to insert into
/// @param[in] New The string to insert
/// @param[in] Index The index of String to insert New at
///
/// @return The modified string
extern PCHAR CmnInsertString(_In_z_ PCSTR String, _In_z_ PCSTR New, _In_ SIZE_T Index);

/// @brief Append a string
///
/// @param[in] String The string to append onto
/// @param[in] New The string to append
///
/// @return The modified string
extern PCHAR CmnAppendString(_In_z_ PCSTR String, _In_z_ PCSTR New);

/// @brief Duplicate a string
///
/// @param[in] String The string to duplicate
/// @param[in] Count The number of characters to copy (0 means all)
///
/// @return A duplicate of the string or NULL
extern PCHAR CmnDuplicateString(_In_z_ PCSTR String, _In_ SIZE_T Count);

/// @brief This routine displays an error message and terminates the program.
///
/// @param[in] ShutdownFirst Whether to attempt a call to CmnShutdown
/// @param[in] File The file where the error was
/// @param[in] Line The line where the error was
/// @param[in] Message The error message.
/// @param[in] ...     The arguments to the error message.
_Noreturn extern VOID CmnErrorEx(_In_ BOOLEAN ShutdownFirst, _In_ PCSTR File, _In_ UINT64 Line, _In_z_ _Printf_format_string_ PCSTR Message, ...);

/// @brief This routine displays an error message and terminates the program.
#define CmnError(...) CmnErrorEx(TRUE, __FILE__, __LINE__, __VA_ARGS__)

/// @brief This routine is a modified version of ReactOS's CommandLineToArgvW
///        that is adjusted to convert ASCII instead of Unicode command lines
///        and use CmnAlloc and CmnFree instead of LocalAlloc and LocalFree.
///
/// @param lpCmdline The command line to parse
/// @param numargs   This parameter receives the number of arguments parsed
///
/// @return An array of parsed arguments
extern PSTR *CmnParseCommandline(_In_ PCSTR lpCmdline, _Out_ PUINT32 numargs);
