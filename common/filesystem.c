/// @file filesystem.c
///
/// @brief This file implements the filesystem library.
///
/// @copyright (c) Randomcode Developers 2024

#include "filesystem.h"
#include "packfile.h"

typedef enum FILESYSTEM_SOURCE_TYPE
{
    FsSourceTypeDirectory,
    FsSourceTypePackFile,
    FsSourceTypeCount
} FILESYSTEM_SOURCE_TYPE, *PFILESYSTEM_SOURCE_TYPE;

typedef struct FILESYSTEM_SOURCE
{
    FILESYSTEM_SOURCE_TYPE Type;
    PCHAR Path;
    PVOID Handle; // for things other than directories

    BOOLEAN (*HasFile)(_In_ PVOID Handle, _In_z_ PCSTR Path);
    UINT64 (*GetFileSize)(_In_ PVOID Handle, _In_z_ PCSTR Path);
    PVOID(*ReadFile)
    (_In_ PVOID Handle, _In_z_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount, _Out_ PUINT64 ReadAmount,
     _In_ UINT64 Extra);
} FILESYSTEM_SOURCE, *PFILESYSTEM_SOURCE;

PFILESYSTEM_SOURCE FsSources;

static BOOLEAN PhysFsHasFile(_In_ PVOID Handle, _In_z_ PCSTR Path)
{
    BOOLEAN Exists = FALSE;

    PCHAR FullPath =
        CmnFormatString("%s%s%s", Handle ? ((PFILESYSTEM_SOURCE)Handle)->Path : "", Handle ? "/" : "", Path);
    PCHAR FixedFullPath = PlatFixPath(FullPath);
    CmnFree(FullPath);

    FILE *File = fopen(FixedFullPath, "r");
    if (File || (!File && errno != ENOENT)) // Should be about right
    {
        Exists = TRUE;
        if (File)
        {
            fclose(File);
        }
    }

    CmnFree(FixedFullPath);

    return Exists;
}

static UINT64 PhysFsGetFileSize(_In_ PVOID Handle, _In_z_ PCSTR Path)
{
    return PlatGetFileSize(Path);
}

BOOLEAN FsCreateDirectory(_In_z_ PCSTR Path)
{
    LogTrace("Creating directory %s", Path);
    return PlatCreateDirectory(Path);
}

static PVOID PhysFsReadFile(_In_ PVOID Handle, _In_z_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount,
                            _Out_ PUINT64 ReadAmount, _In_ UINT64 Extra)
{
    FILE *File;
    PVOID Buffer;
    UINT64 Size;
    UINT64 Read;

    if (!ReadAmount)
    {
        return NULL;
    }

    LogTrace("Reading up to %zu byte(s) (+%zu) of file %s starting at 0x%llX", MaxAmount, Extra, Path, (UINT64)Offset);
    File = fopen(Path, "rb");
    if (!File)
    {
        LogWarning("Failed to open file %s: %s", Path, strerror(errno));
        *ReadAmount = 0;
        return NULL;
    }

    if (MaxAmount > 0)
    {
        Size = MaxAmount + Extra;
    }
    else
    {
        Size = PhysFsGetFileSize(Handle, Path) + Extra;
    }
    Buffer = CmnAlloc(Size, 1);
    if (!Buffer)
    {
        LogWarning("Failed to allocate data for file %s: %s", Path, strerror(errno));
        fclose(File);
        *ReadAmount = 0;
        return NULL;
    }

    fseeko64(File, Offset, SEEK_SET);

    Read = fread(Buffer, 1, Size, File);
    if (Read != Size - Extra)
    {
        LogWarning("Failed to read file %s: %s", Path, strerror(errno));
        fclose(File);
        *ReadAmount = 0;
        CmnFree(Buffer);
        return NULL;
    }

    fclose(File);
    *ReadAmount = Read;
    return Buffer;
}

BOOLEAN FsWriteFile(_In_z_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data, _In_ UINT64 Size, _In_ BOOLEAN Append)
{
    FILE *File;
    BOOLEAN Success;

    LogTrace("%s %zu byte(s) to file %s (%s)", Append ? "Appending" : "Writing", Size, Path, Path);
    File = fopen(Path, Append ? "ab" : "wb");
    if (!File)
    {
        LogWarning("Failed to open file %s (%s): %s", Path, Path, strerror(errno));
        return FALSE;
    }

    Success = fwrite(Data, 1, Size, File) == Size;

    fclose(File);

    return Success;
}

VOID FsAddDirectorySource(_In_z_ PCSTR Path)
{
    if (!Path)
    {
        return;
    }

    FILESYSTEM_SOURCE Source = {0};
    Source.Type = FsSourceTypeDirectory;
    Source.Path = CmnDuplicateString(Path, 0);

    Source.HasFile = PhysFsHasFile;
    Source.GetFileSize = PhysFsGetFileSize;
    Source.ReadFile = PhysFsReadFile;

    LogDebug("Adding directory source %s", Source.Path);

    stbds_arrput(FsSources, Source);
    (&stbds_arrlast(FsSources))->Handle = &stbds_arrlast(FsSources);
}

VOID FsAddPackSource(_In_z_ PCSTR Path)
{
    if (!Path)
    {
        return;
    }

    FILESYSTEM_SOURCE Source = {0};
    Source.Type = FsSourceTypePackFile;
    Source.Path = CmnDuplicateString(Path, 0);
    Source.Handle = PackLoad(Path);
    if (!Source.Handle)
    {
        return;
    }

    Source.HasFile = PackHasFile;
    Source.GetFileSize = PackGetFileSize;
    Source.ReadFile = PackReadFile;

    LogDebug("Adding pack source %s", Source.Path);

    stbds_arrput(FsSources, Source);
}

static PFILESYSTEM_SOURCE FindFile(_In_z_ PCSTR Path)
{
    // TODO: optimize?
    for (SIZE_T i = 0; i < stbds_arrlenu(FsSources); i++)
    {
        if (FsSources[i].HasFile(FsSources[i].Handle, Path))
        {
            LogDebug("Found %s in %s", Path, FsSources[i].Path);
            return &FsSources[i];
        }
    }

    return NULL;
}

#define X(ReturnType, Name, Params, ExtraBefore, ExtraCondition, ExtraAfter, ...)                                      \
    ReturnType Fs##Name Params                                                                                         \
    {                                                                                                                  \
        ExtraBefore;                                                                                                   \
        PFILESYSTEM_SOURCE Source = Raw ? NULL : FindFile(Path);                                                       \
        if (Source ExtraCondition)                                                                                     \
        {                                                                                                              \
            PCHAR FullPath = CmnFormatString("%s/%s", Source->Path, Path);                                             \
            PCHAR FixedFullPath = PlatFixPath(FullPath);                                                               \
            CmnFree(FullPath);                                                                                         \
            ReturnType Return = Source->Name(Source->Handle, __VA_ARGS__);                                             \
            CmnFree(FixedFullPath);                                                                                    \
            return Return;                                                                                             \
        }                                                                                                              \
        else if (Raw ExtraCondition)                                                                                   \
        {                                                                                                              \
            PCHAR FixedFullPath = PlatFixPath(Path);                                                                   \
            ReturnType Return = PhysFs##Name(NULL, __VA_ARGS__);                                                       \
            return Return;                                                                                             \
        }                                                                                                              \
        ExtraAfter;                                                                                                    \
    }

X(
    UINT64, GetFileSize, (_In_ BOOLEAN Raw, _In_z_ PCSTR Path), {}, , { return 0; }, FixedFullPath)
X(
    PVOID, ReadFile,
    (_In_ BOOLEAN Raw, _In_z_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount, _Out_ PUINT64 ReadAmount,
     _In_ UINT64 Extra),
    {}, &&ReadAmount,
    {
        *ReadAmount = 0;
        return NULL;
    },
    FixedFullPath, Offset, MaxAmount, ReadAmount, Extra)

#undef X
