#include "packfile.h"

static PCHAR GetDirectoryPath(_In_z_ PCSTR BasePath)
{
    PCSTR Extension = strrchr(BasePath, '.');
    if (!Extension)
    {
        return CmnAppendString(BasePath, "_dir" PACKFILE_EXTENSION);
    }
    else
    {
        return CmnInsertString(BasePath, "_dir", Extension - BasePath);
    }
}

static PCHAR GetArchivePath(_In_z_ PCSTR BasePath, _In_ UINT16 Index)
{
    PCSTR Extension = strrchr(BasePath, '.');
    if (!Extension)
    {
        return CmnAppendString(BasePath, CmnFormatTempString("_%02hu" PACKFILE_EXTENSION, Index));
    }
    else
    {
        return CmnInsertString(BasePath, CmnFormatTempString("_%02hu" PACKFILE_EXTENSION, Index), Extension - BasePath);
    }
}

PPACKFILE PackCreate(_In_z_ PCSTR Path)
{
    PPACKFILE Pack = CmnAlloc(1, sizeof(PACKFILE));
    if (!Pack)
    {
        LogError("Failed to allocate pack file: %s", strerror(errno));
        return NULL;
    }

    Pack->Path = CmnDuplicateString(Path, 0);
    Pack->Header.Signature = PACKFILE_SIGNATURE;
    Pack->Header.Version = PACKFILE_FORMAT_VERSION;

    return Pack;
}

BOOLEAN PackSave(_Inout_ PVOID Handle, _In_opt_z_ PCSTR Path)
{
    if (!Handle)
    {
        return FALSE;
    }

    PPACKFILE Pack = Handle;
    if (Path)
    {
        CmnFree(Pack->Path);
        Pack->Path = PlatFixPath(Path);
    }

    PCHAR DirectoryPath = GetDirectoryPath(Pack->Path);
    LogInfo("Saving pack file directory to %s", DirectoryPath);

    Pack->Header.ArchiveCount = Pack->CurrentArchive + 1;
    Pack->Header.TreeSize = 0;
    for (UINT64 i = 0; i < stbds_shlenu(Pack->Entries); i++)
    {
        // Update just in case it changed somehow
        Pack->Entries[i].value.PathLength = (UINT16)strlen(Pack->Entries[i].key);
        Pack->Header.TreeSize += sizeof(PACKFILE_ENTRY) + Pack->Entries[i].value.PathLength;
    }

    Pack->Header.ArchiveCount = Pack->CurrentArchive + 1;
    Pack->Header.LastArchiveLength = Pack->CurrentOffset;
    FsWriteFile(DirectoryPath, &Pack->Header, sizeof(PACKFILE_HEADER), FALSE);
    for (UINT64 i = 0; i < stbds_shlenu(Pack->Entries); i++)
    {
        FsWriteFile(DirectoryPath, &Pack->Entries[i].value, sizeof(PACKFILE_ENTRY), TRUE);
        FsWriteFile(DirectoryPath, Pack->Entries[i].key, Pack->Entries[i].value.PathLength, TRUE);
    }

    CmnFree(DirectoryPath);

    return TRUE;
}

PPACKFILE PackLoad(_In_z_ PCSTR DirectoryPath)
{
    if (!DirectoryPath)
    {
        return NULL;
    }

    PCHAR FixedPath = PlatFixPath(DirectoryPath);
    PCHAR Dir = strstr(FixedPath, "_dir");
    PCHAR Path;
    if (Dir)
    {
        *Dir = 0;
        Path = CmnFormatString("%s%s", FixedPath, Dir + 4);
    }
    else
    {
        Path = CmnDuplicateString(FixedPath, 0);
    }

    CmnFree(FixedPath);

    LogInfo("Loading pack file %s", Path);

    PCHAR RealDirectoryPath = GetDirectoryPath(Path);
    UINT64 DirectorySize = 0;
    PBYTE DirectoryRaw = FsReadFile(TRUE, RealDirectoryPath, 0, 0, &DirectorySize, 0);
    if (!DirectoryRaw)
    {
        LogError("Failed to read pack file directory %s", RealDirectoryPath);
        goto Error;
    }

    if (DirectorySize < sizeof(PACKFILE_HEADER))
    {
        LogError("Pack file directory is not large enough to contain a header");
        goto Error;
    }

    PPACKFILE Pack = CmnAlloc(1, sizeof(PACKFILE));
    if (!Pack)
    {
        LogError("Failed to allocate pack file structure");
        goto Error;
    }

    Pack->Header = *(PPACKFILE_HEADER)DirectoryRaw;
    if (Pack->Header.Signature != PACKFILE_SIGNATURE || Pack->Header.Version != PACKFILE_FORMAT_VERSION)
    {
        LogError("Pack file is invalid");
        goto Error;
    }

    PPACKFILE_ENTRY Entry = (PPACKFILE_ENTRY)(DirectoryRaw + sizeof(PACKFILE_HEADER));
    while ((PBYTE)Entry + Entry->PathLength < DirectoryRaw + DirectorySize)
    {
        PCHAR EntryPath = CmnFormatTempString("%.*s", Entry->PathLength, (PCSTR)(Entry + 1));
        stbds_shput(Pack->Entries, EntryPath, *Entry);
        Entry = (PPACKFILE_ENTRY)(EntryPath + Entry->PathLength);
    }

    Pack->CurrentArchive = Pack->Header.ArchiveCount - 1;
    Pack->CurrentOffset = Pack->Header.LastArchiveLength;
    Pack->Path = Path;

    CmnFree(DirectoryRaw);
    CmnFree(RealDirectoryPath);

    return Pack;

Error:
    CmnFree(RealDirectoryPath);
    CmnFree(DirectoryRaw);
    CmnFree(Path);

    return NULL;
}

VOID PackFree(_Inout_ PVOID Handle)
{
    if (Handle)
    {
        PPACKFILE Pack = Handle;
        stbds_arrfree(Pack->Entries);
        CmnFree(Pack->Path);
    }
}

BOOLEAN PackHasFile(_In_ PVOID Handle, _In_z_ PCSTR Path)
{
    PPACKFILE Pack = Handle;
    if (Pack)
    {
        return stbds_shgetp(Pack->Entries, Path) != NULL;
    }

    return FALSE;
}

UINT64 PackGetFileSize(_In_ PVOID Handle, _In_z_ PCSTR Path)
{
    PPACKFILE Pack = Handle;
    if (Pack)
    {
        // "Default" return value is just zero initialized, so this works either way
        return stbds_shget(Pack->Entries, Path).Size;
    }

    return 0;
}

PVOID PackReadFile(_In_ PVOID Handle, _In_z_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount,
                   _Out_ PUINT64 ReadAmount, _In_ UINT64 Extra)
{
    PPACKFILE Pack = Handle;
    if (!Pack)
    {
        return NULL;
    }

    LogInfo("Reading file %s from pack %s", Path, Pack->Path);

    PPACKFILE_ENTRY_MAP Pair = stbds_shgetp(Pack->Entries, Path);
    if (!Pair)
    {
        LogError("File does not exist");
        return NULL;
    }

    PPACKFILE_ENTRY Entry = &Pair->value;

    PBYTE CompressedData = CmnAlloc(Entry->CompressedSize, 1);
    if (!CompressedData)
    {
        LogError("Failed to allocate %zu bytes: %s", Entry->CompressedSize, strerror(errno));
        return NULL;
    }

    UINT64 TotalOffset = 0;
    UINT64 CurrentOffset = 0;
    UINT64 SizeToRead = Entry->CompressedSize;
    UINT16 CurrentArchive = Entry->ArchiveIndex;
    while (SizeToRead > 0)
    {
        PCHAR ArchivePath = GetArchivePath(Pack->Path, CurrentArchive);
        UINT64 Read = PURPL_MIN(PACKFILE_MAX_CHUNK_SIZE - Pack->CurrentOffset, SizeToRead);
        PVOID Data = FsReadFile(TRUE, ArchivePath, CurrentOffset, Read, &Read, 0);
        if (!Data)
        {
            LogError("Failed to read file from pack");
            CmnFree(CompressedData);
            CmnFree(ArchivePath);
            CmnFree(Data);
            return NULL;
        }
        CmnFree(ArchivePath);
        memcpy(CompressedData + TotalOffset, Data, Read);
        CmnFree(Data);
        SizeToRead -= Read;
        TotalOffset += Read;
        CurrentOffset += Read;
        if (Pack->CurrentOffset > PACKFILE_MAX_CHUNK_SIZE)
        {
            CurrentArchive++;
            CurrentOffset = 0;
        }
    }

    XXH128_hash_t CompressedHash = XXH3_128bits(CompressedData, Entry->CompressedSize);
    if (memcmp(&CompressedHash, &Entry->CompressedHash, sizeof(XXH128_hash_t)) != 0)
    {
        LogError("Compressed data hash does not match: got %llX%llX, expected %llX%llX", CompressedHash.high64,
                 CompressedHash.low64, Entry->CompressedHash.high64, Entry->CompressedHash.low64);
        CmnFree(CompressedData);
        return NULL;
    }

    PBYTE Data = CmnAlloc(Entry->Size, 1);
    if (!Data)
    {
        LogError("Failed to allocate %zu bytes: %s", Entry->Size, strerror(errno));
        CmnFree(CompressedData);
    }

    SIZE_T DecompressedSize = ZSTD_decompress(Data, Entry->Size, CompressedData, Entry->CompressedSize);
    if (ZSTD_isError(DecompressedSize) || DecompressedSize != Entry->Size)
    {
        if (ZSTD_isError(DecompressedSize))
        {
            LogError("Decompression failed: %s", ZSTD_getErrorName(DecompressedSize));
        }
        else
        {
            LogError("Decompressed size does not match: got %s, expected %s", CmnFormatSize(DecompressedSize),
                     CmnFormatTempString("%s", CmnFormatSize(Entry->Size)));
        }
        CmnFree(CompressedData);
        CmnFree(Data);
    }

    XXH128_hash_t Hash = XXH3_128bits(Data, Entry->Size);
    if (memcmp(&Hash, &Entry->Hash, sizeof(XXH128_hash_t)) != 0)
    {
        LogError("Compressed hash does not match: got %llX%llX, expected %llX%llX", Hash.high64, Hash.low64,
                 Entry->Hash.high64, Entry->Hash.low64);
        CmnFree(Data);
        return NULL;
    }

    CmnFree(CompressedData);

    return Data;
}

BOOLEAN PackAddFile(_Inout_ PVOID Handle, _In_z_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data, _In_ UINT64 Size)
{
    PPACKFILE Pack = Handle;
    if (!Pack)
    {
        return FALSE;
    }

    SIZE_T CompressedSize = ZSTD_compressBound(Size);
    PBYTE CompressedData = CmnAlloc(CompressedSize, 1);
    if (!CompressedData)
    {
        LogError("Failed to add %s file as %s to pack %s: %s", CmnFormatSize(Size), Path, Pack->Path, strerror(errno));
        return FALSE;
    }

    CompressedSize = ZSTD_compress(CompressedData, CompressedSize, Data, Size, PACKFILE_COMPRESSION_LEVEL);
    if (ZSTD_isError(CompressedSize))
    {
        LogError("Failed to compress data: %s", ZSTD_getErrorName(CompressedSize));
        CmnFree(CompressedData);
        return FALSE;
    }

    LogDebug("Adding %s (%s compressed) file as %s to pack %s", CmnFormatSize(Size),
             CmnFormatTempString("%s", CmnFormatSize(CompressedSize)), Path,
             Pack->Path); // TODO: there has to be a better way of dealing with static buffers

    PACKFILE_ENTRY Entry = {0};
    Entry.Hash = XXH3_128bits(Data, Size);
    Entry.CompressedHash = XXH3_128bits(CompressedData, CompressedSize);
    Entry.ArchiveIndex = Pack->CurrentArchive;
    Entry.Offset = Pack->CurrentOffset;
    Entry.Size = Size;
    Entry.CompressedSize = CompressedSize;

    stbds_shput(Pack->Entries, Path, Entry);

    UINT64 DataOffset = 0;
    UINT64 SizeToWrite = CompressedSize;
    while (SizeToWrite > 0)
    {
        PCHAR ArchivePath = GetArchivePath(Pack->Path, Pack->CurrentArchive);
        UINT64 Written = PURPL_MIN(PACKFILE_MAX_CHUNK_SIZE - Pack->CurrentOffset, SizeToWrite);
        if (!FsWriteFile(ArchivePath, CompressedData + DataOffset, Written, TRUE))
        {
            LogError("Failed to add file to pack");
            CmnFree(CompressedData);
            CmnFree(ArchivePath);
            return FALSE;
        }
        CmnFree(ArchivePath);
        SizeToWrite -= Written;
        DataOffset += Written;
        Pack->CurrentOffset += Written;
        if (Pack->CurrentOffset > PACKFILE_MAX_CHUNK_SIZE)
        {
            Pack->CurrentArchive++;
            Pack->CurrentOffset = 0;
        }
    }

    CmnFree(CompressedData);

    return TRUE;
}
