/*++

Copyright (c) 2024 Randomcode Developers

Module Name:

    launcher.c

Abstract:

    This file implements the entrypoint for Unix-like operating systems.

--*/

#include "purpl/purpl.h"

#include "common/common.h"

PSP_MODULE_INFO(PURPL_EXECUTABLE_NAME, 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

unsigned int sce_newlib_nocreate_thread_in_start = 1;
const char *sce_newlib_main_thread_name = "main";

extern VOID InitializeMainThread(_In_ PFN_THREAD_START ThreadStart);

INT main(INT argc, PCHAR *argv)
/*++

Routine:

    This routine is the entrypoint for Unix-like systems.

Arguments:

    argc - The number of arguments.

    argv - Array of command line arguments.

Return Value:

    An appropriate errno code.

--*/
{
    INT Result;
    
    InitializeMainThread((PFN_THREAD_START)main);

    Result = PurplMain(argv, argc);

    return Result;
}
