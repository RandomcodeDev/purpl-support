/// @file filesystem.c
///
/// @brief This file implements the filesystem library.
///
/// @copyright (c) Randomcode Developers 2024

#include "filesystem.h"

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
    BOOLEAN ReadOnly;

    BOOLEAN (*HasFile)(_In_ struct FILESYSTEM_SOURCE *Source, _In_ PCSTR Path);
    UINT64 (*GetFileSize)(_In_ PCSTR Path);
    PVOID(*ReadFile)
    (_In_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount, _Out_ PUINT64 ReadAmount, _In_ UINT64 Extra);
    BOOLEAN (*WriteFile)(_In_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data, _In_ UINT64 Size, _In_ BOOLEAN Append);
} FILESYSTEM_SOURCE, *PFILESYSTEM_SOURCE;

PFILESYSTEM_SOURCE FsSources;

static BOOLEAN PhysFsHasFile(_In_ PFILESYSTEM_SOURCE Source, _In_ PCSTR Path)
{
    BOOLEAN Exists = FALSE;

    PCHAR FullPath = CmnFormatString("%s/%s", Source->Path, Path);
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

static UINT64 PhysFsGetFileSize(_In_ PCSTR Path)
{
    return PlatGetFileSize(Path);
}

BOOLEAN FsCreateDirectory(_In_ PCSTR Path)
{
    LogTrace("Creating directory %s", Path);
    return PlatCreateDirectory(Path);
}

static PVOID PhysFsReadFile(_In_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount, _Out_ PUINT64 ReadAmount,
                            _In_ UINT64 Extra)
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
        Size = PhysFsGetFileSize(Path) + Extra;
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

static BOOLEAN PhysFsWriteFile(_In_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data, _In_ UINT64 Size,
                               _In_ BOOLEAN Append)
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

VOID FsAddDirectorySource(_In_ PCSTR Path, _In_ BOOLEAN ReadOnly)
{
    if (!Path)
    {
        return;
    }

    FILESYSTEM_SOURCE Source = {0};
    Source.Type = FsSourceTypeDirectory;
    Source.Path = CmnFormatString("%s", Path);

    Source.HasFile = PhysFsHasFile;
    Source.GetFileSize = PhysFsGetFileSize;
    Source.ReadFile = PhysFsReadFile;
    Source.WriteFile = PhysFsWriteFile;

    LogDebug("Adding directory source %s", Source.Path);

    stbds_arrput(FsSources, Source);
}

VOID FsAddPackSource(_In_ PCSTR Path, _In_ BOOLEAN ReadOnly)
{
    if (!Path)
    {
        return;
    }

    FILESYSTEM_SOURCE Source = {0};
    Source.Type = FsSourceTypePackFile;
    Source.Path = CmnFormatString("%s", Path);
    // Source.Handle = PackLoad(Path);
    if (!Source.Handle)
    {
        return;
    }

    // Source.HasFile = PackHasFile;
    // Source.GetFileSize = PackGetFileSize;
    // Source.ReadFile = PackReadFile;
    // Source.WriteFile = PackWriteFile;

    LogDebug("Adding pack source %s", Source.Path);

    stbds_arrput(FsSources, Source);
}

static PFILESYSTEM_SOURCE FindFile(_In_ PCSTR Path)
{
    // TODO: optimize?
    for (SIZE_T i = 0; i < stbds_arrlenu(FsSources); i++)
    {
        if (FsSources[i].HasFile(&FsSources[i], Path))
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
        PFILESYSTEM_SOURCE Source = FindFile(Path);                                                                    \
        if (Source ExtraCondition)                                                                                     \
        {                                                                                                              \
            PCHAR FullPath = CmnFormatString("%s/%s", Source->Path, Path);                                             \
            PCHAR FixedFullPath = PlatFixPath(FullPath);                                                               \
            CmnFree(FullPath);                                                                                         \
            ReturnType Return = Source->Name(__VA_ARGS__);                                                             \
            CmnFree(FixedFullPath);                                                                                    \
            return Return;                                                                                             \
        }                                                                                                              \
        ExtraAfter;                                                                                                    \
    }

X(
    UINT64, GetFileSize, (_In_ PCSTR Path), {}, , { return 0; }, FixedFullPath)
X(
    PVOID, ReadFile,
    (_In_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount, _Out_ PUINT64 ReadAmount, _In_ UINT64 Extra), {}, ,
    {
        if (!Source)
        {
            *ReadAmount = 0;
        }
    },
    FixedFullPath, Offset, MaxAmount, ReadAmount, Extra)
X(
    BOOLEAN, WriteFile, (_In_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data, _In_ UINT64 Size, _In_ BOOLEAN Append), {},
    &&!Source->ReadOnly, { return FALSE; }, FixedFullPath, Data, Size, Append)

#undef X
