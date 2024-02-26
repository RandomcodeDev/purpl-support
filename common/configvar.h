/// @file configvar.h
///
/// @brief This file defines the interface for configuration variables
///
/// @copyright (c) Randomcode Developers 2024

#pragma once

#include "common/common.h"

/// @brief Data type of the variable
typedef enum CONFIGVAR_TYPE
{
    ConfigVarTypeBoolean,
    ConfigVarTypeInt,
    ConfigVarTypeFloat,
    ConfigVarTypeString,
    ConfigVarTypeCount
} CONFIGVAR_TYPE, *PCONFIGVAR_TYPE;

/// @brief

///
typedef struct CONFIGVAR
{
    CHAR Name[32];
    CONFIGVAR_TYPE Type;
    union {
        BOOLEAN BooleanValue;
        UINT64 IntValue;
        DOUBLE FloatValue;
        CHAR StringValue[64];
    };
    union {
        UINT8 Bitflags;
        struct
        {
            UINT8 Changed : 1;
            UINT8 Side : 2; // 0 = both, 1 = client, 2 = server
            UINT8 Cheat : 1;
        };
    };
} CONFIGVAR, *PCONFIGVAR;

VOID CfgInitializeVariable(_Out_ PCONFIGVAR Variable, _In_ PCSTR Name, _In_ CONST PVOID Value,
                           _In_ CONFIGVAR_TYPE Type);
