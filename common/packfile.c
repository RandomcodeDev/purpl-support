#include "packfile.h"

PPACKFILE PackCreate(_In_z_ PCSTR DirectoryPath)
{
    PPACKFILE Pack = CmnAlloc(1, sizeof(PACKFILE));
    if (!Pack)
    {
        LogError("Failed to allocate pack file: %s", strerror(errno));
        return NULL;
    }

    Pack->Path = PlatFixPath(DirectoryPath);
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

    LogInfo("Saving pack file to %s", Pack->Path);
    for (UINT64 i = 0; i < stbds_shlenu(Pack->DirectoryEntries); i++)
    {

    }
}

PPACKFILE PackLoad(_In_z_ PCSTR DirectoryPath)
{
}

VOID PackFree(_Inout_ PVOID Handle)
{
    if (Handle)
    {
        PPACKFILE Pack = Handle;
        stbds_arrfree(Pack->DirectoryEntries);
        if (Pack->Path)
        {
            CmnFree(Pack->Path);
        }
    }
}

BOOLEAN PackHasFile(_In_ PVOID Handle, _In_z_ PCSTR Path)
{
    PPACKFILE Pack = Handle;
    if (Pack)
    {
        return stbds_shgetp(Pack->DirectoryEntries, Path) != NULL;
    }
}

UINT64 PackGetFileSize(_In_ PVOID Handle, _In_z_ PCSTR Path)
{
    PPACKFILE Pack = Handle;
    if (Pack)
    {
        // "Default" return value is just zero initialized, so this works either way
        return stbds_shget(Pack->DirectoryEntries, Path).Length;
    }
}

PVOID PackReadFile(_In_ PVOID Handle, _In_z_ PCSTR Path, _In_ UINT64 Offset, _In_ UINT64 MaxAmount,
                   _Out_ PUINT64 ReadAmount, _In_ UINT64 Extra)
{
}

BOOLEAN PackAddFile(_Inout_ PVOID Handle, _In_z_ PCSTR Path, _In_reads_bytes_(Size) PVOID Data, _In_ UINT64 Size)
{
}
