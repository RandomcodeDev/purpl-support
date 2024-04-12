/// @file filesystem.h
///
/// @brief This file contains definitions for the common filesystem library.
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "purpl/purpl.h"

#include "platform/platform.h"

#include "alloc.h"
#include "common.h"
#include "log.h"

/// @brief Adds a directory source to the filesystem
///
/// @param[in] Path The path of the directory
extern VOID FsAddDirectorySource(_In_z_ PCSTR Path);

/// @brief Adds a pack file source to the filesystem
///
/// @param[in] Path The path of the pack file
extern VOID FsAddPackSource(_In_z_ PCSTR Path);

/// @brief Checks if a file exists
///
/// @param[in] Raw Whether to skip source abstraction
/// @param[in] Path The path to the file
///
/// @return Whether the file exists
extern BOOLEAN FsHasFile(_In_ BOOLEAN Raw, _In_z_ PCSTR Path);

/// @brief Gets the size of a file
///
///
/// @param[in] Raw Whether to skip the source abstraction
/// @param[in] Path The path to the file
///
/// @return The size of the file in bytes
extern UINT64 FsGetFileSize(_In_ BOOLEAN Raw, _In_z_ PCSTR Path);

/// @brief Creates a directory
///
/// @param[in] Path The path to the directory
///
/// @return Whether the creation succeeded
extern BOOLEAN FsCreateDirectory(_In_z_ PCSTR Path);

/// @brief This routine reads a file into a buffer which it allocates.
///
/// @param[in] Raw Whether to skip the source abstraction
/// @param[in] Path The path to the file to read.
/// @param[in] Offset The offset from the start of the file.
/// @param[in] MaxAmount The maximum number of bytes to read, or zero for the whole file.
/// @param[out] ReadAmount A pointer to a variable that receives the number of bytes read from the file.
/// @param[in] Extra Number of extra bytes to allocate.
///
/// @return A pointer to a buffer containing the file's contents, or NULL.
extern PVOID FsReadFile(_In_ BOOLEAN Raw, _In_z_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount,
                        _Out_ PUINT64 ReadAmount, _In_ UINT64 Extra);

/// @brief Write to a file
///
/// @param[in] Path The path to the file
/// @param[in] Data The data to write to the file
/// @param[in] Size The size of the data
/// @param[in] Append Whether to append to the file or overwrite it (either way, it will be created)
///
/// @return Whether the write succeeded
extern BOOLEAN FsWriteFile(_In_z_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data, _In_ UINT64 Size,
                           _In_ BOOLEAN Append);
