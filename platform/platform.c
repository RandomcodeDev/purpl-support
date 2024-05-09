#include "common/common.h"

#include "platform.h"

PCSTR PlatGetCpuName(VOID)
{
    static CHAR Name[128] = {0};

    if (!strlen(Name))
    {
#ifdef PURPL_X86
        CHAR Brand[13] = {0};
        CHAR Model[65] = {0};
        UINT32 Registers[16] = {0};

#ifdef _MSC_VER
        __cpuidex(Registers, 0, 0);

        __cpuidex(Registers + 4, 0x80000000, 0);
        if (Registers[4] >= 0x80000004)
        {
            __cpuidex(Registers + 4, 0x80000002, 0);
            __cpuidex(Registers + 8, 0x80000003, 0);
            __cpuidex(Registers + 12, 0x80000004, 0);
        }
        else
        {
            LogDebug("CPUID 0x80000004 unsupported");
        }
#else

#endif

        *(PUINT32)Brand = Registers[1];
        *(PUINT32)(Brand + 4) = Registers[3];
        *(PUINT32)(Brand + 8) = Registers[2];
        memcpy(Model, Registers + 4, 64);

        snprintf(Name, PURPL_ARRAYSIZE(Name), "%s %s", Brand, Model);
#endif
    }

    return Name;
}
