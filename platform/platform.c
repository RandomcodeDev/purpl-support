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
        __cpuid(Registers, 0);

        __cpuid(Registers + 4, 0x80000000);
        if (Registers[4] >= 0x80000004)
        {
            __cpuid(Registers + 4, 0x80000002);
            __cpuid(Registers + 8, 0x80000003);
            __cpuid(Registers + 12, 0x80000004);
        }
        else
        {
            LogDebug("CPUID 0x80000004 unsupported");
        }
#else
        __cpuid(0, Registers[0], Registers[1], Registers[2], Registers[3]);

        __cpuid(0x80000000, Registers[4], Registers[5], Registers[6], Registers[7]);
        if (Registers[4] >= 0x80000004)
        {
            __cpuid(0x80000002, Registers[4], Registers[5], Registers[6], Registers[7]);
            __cpuid(0x80000003, Registers[8], Registers[9], Registers[10], Registers[11]);
            __cpuid(0x80000004, Registers[12], Registers[13], Registers[14], Registers[15]);
        }
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
