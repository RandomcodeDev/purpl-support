/// @file packfile.h
///
/// @brief This file contains definitions for the pack file format. This is, essentially, a very scuffed, simplified
/// version of Valve's VPK format.
///
/// Also, the directory tree is simplified due to limitations in stb_ds, hence the version change. This also allows for
/// other changes, such as xxhash and 64-bit sizes/offsets.
///
/// All the functions take PVOID instead of PPACKFILE because of how the filesystem abstraction is implemented, and
/// casting function pointers causes so many warnings, even in safe cases.
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "purpl/purpl.h"

#include "platform/platform.h"

#include "alloc.h"
#include "common.h"
#include "filesystem.h"
#include "log.h"

/// @brief Pack file magic number (little endian)
#define PACKFILE_SIGNATURE 0x55AA1234

/// @brief Pack file format version (4, because 3 is used by some engines)
#define PACKFILE_FORMAT_VERSION 4

/// @brief Maximum chunk size
#define PACKFILE_MAX_CHUNK_SIZE 209715200

/// @brief Extension of a pack file
#define PACKFILE_EXTENSION ".pak"

/// @brief ZSTD compression level of data in pack files
#define PACKFILE_COMPRESSION_LEVEL 9

#pragma pack(push, 1)
/// @brief Pack file directory header
PURPL_MAKE_TAG(struct, PACKFILE_HEADER, {
    UINT32 Signature;
    UINT32 Version;
    UINT32 TreeSize;
    UINT16 ArchiveCount;
    UINT64 LastArchiveLength;
})

/// @brief Pack file directory entry
PURPL_MAKE_TAG(struct, PACKFILE_ENTRY, {
    XXH128_hash_t Hash;
    XXH128_hash_t CompressedHash;
    UINT16 ArchiveIndex;
    UINT64 Offset;
    UINT64 Size;
    UINT64 CompressedSize;
    UINT16 PathLength;
    // on-disk: the path
})
#pragma pack(pop)

PURPL_MAKE_STRING_HASHMAP_ENTRY(PACKFILE_ENTRY_MAP, PACKFILE_ENTRY);

/// @brief A representation of a pack file
PURPL_MAKE_TAG(struct, PACKFILE,
{
    PCHAR Path;
    PACKFILE_HEADER Header;
    PPACKFILE_ENTRY_MAP Entries;
    UINT16 CurrentArchive;
    UINT64 CurrentOffset;
})

/// @brief Create a pack file
///
/// @param[in] Path The path to the pack file
///
/// @return A pack file, or NULL if it couldn't be created
extern PPACKFILE PackCreate(_In_z_ PCSTR Path);

/// @brief Save a pack file
///
/// @param[in] Handle The pack file to save
/// @param[in] Path The path to save the pack file to
///
/// @return Whether the pack file could be saved
extern BOOLEAN PackSave(_Inout_ PVOID Handle, _In_opt_z_ PCSTR Path);

/// @brief Load a pack file
///
/// @param[in] DirectoryPath The path to the directory
///
/// @return A pack file, or NULL if it couldn't be loaded
extern PPACKFILE PackLoad(_In_z_ PCSTR DirectoryPath);

/// @brief Free a pack file
///
/// @param[in,out] Handle The pack file to free
extern VOID PackFree(_Inout_ PVOID Handle);

/// @brief Whether a pack file has a file
///
/// @param[in,out] Handle The pack file
/// @param[in] Path The path to check for
///
/// @return Whether the pack has the file in it
extern BOOLEAN PackHasFile(_In_ PVOID Handle, _In_z_ PCSTR Path);

/// @brief Gets the size of a file
///
/// @param[in] Path The path to the file
///
/// @return The size of the file in bytes
extern UINT64 PackGetFileSize(_In_ PVOID Handle, _In_z_ PCSTR Path);

/// @brief This routine reads a file into a buffer which it allocates.
///
/// @param[in,out] Handle The pack file
/// @param[in] Path       The path to the file to read.
/// @param[in] Offset     The offset from the start of the file.
/// @param[in] MaxAmount  The maximum number of bytes to read, or zero for the whole file.
/// @param[out] ReadAmount A pointer to a variable that receives the number of bytes read from the file.
/// @param[in] Extra      Number of extra bytes to allocate.
///
/// @return A pointer to a buffer containing the file's contents, or NULL.
extern PVOID PackReadFile(_In_ PVOID Handle, _In_z_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount,
                          _Out_ PUINT64 ReadAmount, _In_ UINT64 Extra);

/// @brief Add a file to a pack file
///
/// @param[in,out] Handle The pack file
/// @param[in] Path The path to the file
/// @param[in] Data The data to write to the file
/// @param[in] Size The size of the data
///
/// @return Whether adding the file succeeded
extern BOOLEAN PackAddFile(_Inout_ PVOID Handle, _In_z_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data,
                           _In_ UINT64 Size);
