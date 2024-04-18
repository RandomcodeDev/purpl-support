/*++

Copyright (c) 2024 MobSlicer152

Module Name:

    packtool.c

Abstract:

    This file implements a rudimentary tool for working with pack files.

--*/

#ifdef PURPL_WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"
#include "common/packfile.h"

#include "re.h"

_Noreturn VOID Usage(VOID)
/*++

Routine Description:

    Prints instructions for using the program and exits.

Arguments:

    None.

Return Value:

    Does not return.

--*/
{
    LogInfo("Usage:");
    LogInfo("\tcreate <directory base name> <input> [<input...>]\t\t- Create a pack file");
    LogInfo("\textract <pack directory> [folder]\t\t\t\t\t\t- Extract a pack file");
    LogInfo("\tlist <pack directory> [<regex>] [<-verbose>]\t\t\t- List a pack file's contents");
    exit(EINVAL);
}

static VOID AddFile(_Inout_ PPACKFILE PackFile, _In_z_ PCSTR Path, _In_z_ PCSTR InnerPath)
{
    UINT64 Size = 0;
    PVOID Data = FsReadFile(TRUE, Path, 0, 0, &Size, 0);
    if (Data)
    {
        LogInfo("%s -> %s/%s", Path, PackFile->Path, InnerPath);
        PackAddFile(PackFile, InnerPath, Data, Size);
    }
}

static VOID AddDirectory(_Inout_ PPACKFILE PackFile, _In_ DIR *Directory, _In_z_ PCSTR Path,
                         _In_opt_z_ PCSTR InnerBasePath)
{
    if (Path)
    {
        LogInfo("%s -> %s%s%s", Path, PackFile->Path, InnerBasePath ? "/" : "", InnerBasePath ? InnerBasePath : "");
    }

    struct dirent *Entry;
    while ((Entry = readdir(Directory)))
    {
        if (strncmp(Entry->d_name, ".", Entry->d_namlen) == 0 || strncmp(Entry->d_name, "..", Entry->d_namlen) == 0)
        {
            continue;
        }

        PCHAR FullPath = CmnFormatString("%s/%.*s", Path, (INT)Entry->d_namlen, Entry->d_name);
        PURPL_ASSERT(FullPath != NULL);
        PCHAR InnerPath = CmnFormatString("%s%s%.*s", InnerBasePath ? InnerBasePath : "", InnerBasePath ? "/" : "",
                                          (INT)Entry->d_namlen, Entry->d_name);
        PURPL_ASSERT(InnerPath != NULL);

        if (Entry->d_type == DT_DIR)
        {
            DIR *SubDirectory = opendir(FullPath);
            if (SubDirectory)
            {
                AddDirectory(PackFile, SubDirectory, FullPath, InnerPath);
                closedir(SubDirectory);
            }
        }
        else if (Entry->d_type == DT_REG)
        {
            AddFile(PackFile, FullPath, InnerPath);
        }
        else
        {
            LogInfo("%d", Entry->d_type);
        }

        CmnFree(FullPath);
        CmnFree(InnerPath);
    }
}

static INT Create(_In_ PPACKFILE PackFile, _In_ PCHAR *Arguments, _In_ UINT32 ArgumentCount)
{
    for (UINT32 i = 0; i < ArgumentCount; i++)
    {
        PCHAR Path = PlatFixPath(Arguments[i]);
        PURPL_ASSERT(Path != NULL);
        DIR *Directory = opendir(Path);
        if (Directory)
        {
            AddDirectory(PackFile, Directory, Path, NULL);
        }
        else
        {
            AddFile(PackFile, Path, Path);
        }
        CmnFree(Path);
    }

    PackSave(PackFile, NULL);

    return 0;
}

static INT Extract(_In_ PPACKFILE PackFile, _In_ PCHAR *Arguments, _In_ UINT32 ArgumentCount)
{
    return 0;
}

static INT List(_In_ PPACKFILE PackFile, _In_ PCHAR *Arguments, _In_ UINT32 ArgumentCount)
{
    for (UINT64 i = 0; i < stbds_shlenu(PackFile->Entries); i++)
    {
        PPACKFILE_ENTRY Entry = &PackFile->Entries[i].value;
        LogInfo("%s", PackFile->Entries[i].key);
        LogInfo("\tArchive: %hu", Entry->ArchiveIndex);
        LogInfo("\tOffset: %s", CmnFormatSize(Entry->Offset));
        LogInfo("\tSize: %s", CmnFormatSize(Entry->Size));
        LogInfo("\tCompressed size: %s", CmnFormatSize(Entry->CompressedSize));
        LogInfo("\tHash: 0x%llX%llX", Entry->Hash.low64, Entry->Hash.high64);
        LogInfo("\tCompressed hash: 0x%llX%llX", Entry->CompressedHash.low64, Entry->CompressedHash.high64);
    }

    return 0;
}

//
// Tool mode
//

typedef enum PACKTOOL_MODE
{
    PackToolModeNone,
    PackToolModeCreate,
    PackToolModeExtract,
    PackToolModeList,
    PackToolModeCount
} PACKTOOL_MODE, *PPACKTOOL_MODE;

//
// Functions for each mode
//

typedef INT (*PFNPACKTOOL_OPERATION)(_In_ PPACKFILE PackFile, _In_ PCHAR *Arguments, _In_ UINT32 ArgumentCount);
PFNPACKTOOL_OPERATION Operations[PackToolModeCount] = {
    (PFNPACKTOOL_OPERATION)Usage,
    Create,
    Extract,
    List,
};

INT main(INT argc, PCHAR *argv)
/*++

Routine Description:

    Processes arguments and calls the requested function.

Arguments:

    argc - Number of arguments.

    argv - Array of arguments.

Return Value:

    EXIT_SUCCESS - Success.

    errno value - Failure.

--*/
{
    PACKTOOL_MODE Mode;
    INT Result;

    LogInfo("Purpl Pack Tool v" PURPL_VERSION_STRING
            " (supports pack format v" PURPL_STRINGIZE_EXPAND(PACK_FORMAT_VERSION) ") on %s",
            PlatGetDescription());

    CmnInitialize(NULL, 0);

    if (argc < 3)
    {
        Mode = PackToolModeNone;
    }
    else if (strcmp(argv[1], "create") == 0)
    {
        Mode = PackToolModeCreate;
    }
    else if (strcmp(argv[1], "extract") == 0)
    {
        Mode = PackToolModeExtract;
    }
    else if (strcmp(argv[1], "list") == 0)
    {
        Mode = PackToolModeList;
    }
    else
    {
        Mode = PackToolModeNone;
    }

    PPACKFILE PackFile;

    if (Mode == PackToolModeCreate)
    {
        if (FsHasFile(TRUE, CmnFormatTempString("%s_dir.pak", argv[2])))
        {
            LogWarning("Pack file %s already exists, remove it [Y/N]? ", CmnFormatTempString("%s_dir.pak", argv[2]));
            if (toupper(getchar()) == 'Y')
            {
                LogWarning("Removing file");
                remove(argv[2]);
            }
            else
            {
                LogWarning("Not removing file");
                return EEXIST;
            }
        }
        PackFile = PackCreate(argv[2]);
    }
    else if (Mode != PackToolModeNone)
    {
        PackFile = PackLoad(argv[2]);
    }

    Result = Operations[Mode](PackFile, argv + 3, argc - 3);
    PackFree(PackFile);

    CmnShutdown();

    return Result;
}
