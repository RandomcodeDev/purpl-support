/*++

Copyright (c) 2024 MobSlicer152

Module Name:

    packtool.c

Abstract:

    This file implements a rudimentary tool for working with pack files.

--*/

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
    printf("Usage:\n"
           "\n"
           "\tcreate <packfile> <input> [<input...>]\t\t\t- Create a pack file\n"
           "\textract <packfile> [folder]\t\t\t\t- Extract a pack file\n"
           "\tlist <packfile> [<regex>] [<-verbose>]\t\t\t\t- List a pack file's contents\n");
    exit(EINVAL);
}

INT Create(_In_ PPACKFILE *PackFile, _In_ PCHAR *Arguments)
{

}

INT Extract(_In_ PPACKFILE *PackFile, _In_ PCHAR *Arguments)
{

}

INT List(_In_ PPACKFILE *PackFile, _In_ PCHAR *Arguments)
{

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

    printf("Purpl Pack Tool v" PURPL_VERSION_STRING
           " (supports pack format v" PURPL_STRINGIZE_EXPAND(PACK_FORMAT_VERSION) ") on %s\n\n",
           PlatGetDescription());

    CmnInitialize(NULL, 0);

    if (argc < 4)
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
    else
    {
        Mode = PackToolModeNone;
    }

    PPACKFILE PackFile;

    if (Mode == PackToolModeCreate)
    {
        if (FsHasFile(TRUE, argv[2]))
        {
            printf("Pack file %s already exists, remove it [Y/N]? ", argv[2]);
            if (toupper(getchar()) == 'Y')
            {
                printf("Removing file\n");
                remove(argv[2]);
            }
            else
            {
                printf("Not removing file\n");
                return EEXIST;
            }
        }
        PackFile = PackCreate(argv[2]);
    }
    else if (Mode != PackToolModeNone)
    {
        PackFile = PackLoad(argv[2]);
    }

    Result = Operations[Mode](PackFile, argv + 2, argc - 2);

    CmnShutdown();

    return Result;
}
