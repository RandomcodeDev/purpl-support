/// @file configvar.c
///
/// @brief This file implements the interface for configuration variables
///
/// @copyright (c) Randomcode Developers 2024

#include "configvar.h"

static PCSTR GetSideString(_In_ CONFIGVAR_SIDE Side)
{
    switch (Side)
    {
    case ConfigVarSideBoth:
        return "client and server";
    case ConfigVarSideClientOnly:
        return "client";
    case ConfigVarSideServerOnly:
        return "server";
    }

    return "unknown side";
}

PURPL_MAKE_HASHMAP_ENTRY(CONFIGVAR_MAP, PCHAR, PCONFIGVAR);
PCONFIGVAR_MAP CfgVariables;

VOID CfgDefineVariable(_In_ PCSTR Name, _In_ CONST PVOID DefaultValue, _In_ CONFIGVAR_TYPE Type,
                           _In_ BOOLEAN Static, _In_ CONFIGVAR_SIDE Side, _In_ BOOLEAN Cheat)
{
    if (!Name || Type >= ConfigVarTypeCount || stbds_shget(CfgVariables, Name))
    {
        return;
    }

    PCONFIGVAR Variable = CmnAlloc(1, sizeof(CONFIGVAR));
    PURPL_ASSERT(Variable != NULL);

    Variable->Side = Side & 0b11;
    PSTR SideString = CmnFormatTempString("%s", GetSideString(Variable->Side));
    SideString[0] = (CHAR)toupper(SideString[0]);
    Variable->Cheat = Cheat;
    PCSTR CheatString = Variable->Cheat ? " cheat" : "";
    Variable->Static = Static;
    PCSTR StaticString = Variable->Static ? " static" : "";

    strncpy(Variable->Name, Name, PURPL_ARRAYSIZE(Variable->Name));
    Variable->Type = Type;
    switch (Type)
    {
    case ConfigVarTypeBoolean:
        Variable->Default.Boolean = *(PBOOLEAN)DefaultValue;
        LogInfo("%s%s%s variable %s initialized with boolean value %s", SideString, StaticString, CheatString,
                Variable->Name, Variable->Default.Boolean ? "true" : "false");
        break;
    case ConfigVarTypeInteger:
        Variable->Default.Int = *(PINT64)DefaultValue;
        LogInfo("%s%s%s variable %s initialized with integer value %d", SideString, StaticString, CheatString,
                Variable->Name, Variable->Default.Int);
        break;
    case ConfigVarTypeFloat:
        Variable->Default.Float = *(DOUBLE *)DefaultValue;
        LogInfo("%s%s%s variable %s initialized with float value %f", SideString, StaticString, CheatString,
                Variable->Name, Variable->Default.Float);
        break;
    case ConfigVarTypeString:
        strncpy(Variable->Default.String, DefaultValue, PURPL_ARRAYSIZE(Variable->Default.String));
        LogInfo("%s%s%s variable %s initialized with string value %s", SideString, StaticString, CheatString,
                Variable->Name, Variable->Default.String);
        break;
    default:
        break;
    }

    memcpy(&Variable->Current, &Variable->Default, sizeof(CONFIGVAR_VALUE));

    stbds_shput(CfgVariables, Name, Variable);
}

PCONFIGVAR CfgGetVariable(_In_ PCSTR Name)
{
    PCONFIGVAR Variable = stbds_shget(CfgVariables, Name);
    if (!Variable)
    {
        LogWarning("Unknown variable %s, check the spelling.", Name);
    }

    return Variable;
}

VOID CfgSetVariable(_In_ PCSTR Name, _In_ PVOID Value)
{
    PCONFIGVAR Variable = CfgGetVariable(Name);
    if (!Variable)
    {
        return;
    }

    Variable->Changed = TRUE;
    switch (Variable->Type)
    {
    case ConfigVarTypeBoolean:
        Variable->Current.Boolean = *(PBOOLEAN)Value;
        break;
    case ConfigVarTypeInteger:
        Variable->Current.Int = *(PINT64)Value;
        break;
    case ConfigVarTypeFloat:
        Variable->Current.Float = *(DOUBLE *)Value;
        break;
    case ConfigVarTypeString:
        strncpy(Variable->Current.String, Value, PURPL_ARRAYSIZE(Variable->Default.String));
        break;
    default:
        break;
    }
}
