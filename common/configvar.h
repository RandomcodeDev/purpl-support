/// @file configvar.h
///
/// @brief This file defines the interface for configuration variables
///
/// @copyright (c) Randomcode Developers 2024

#pragma once

#include "common/alloc.h"
#include "common/common.h"

/// @brief Data type of a variable
typedef enum CONFIGVAR_TYPE
{
    ConfigVarTypeBoolean, // BOOLEAN
    ConfigVarTypeInt,     // INT32
    ConfigVarTypeFloat,   // FLOAT
    ConfigVarTypeString,  // String, up to 64 characters including NUL
    ConfigVarTypeCount
} CONFIGVAR_TYPE, *PCONFIGVAR_TYPE;

/// @brief The side of a variable
typedef enum CONFIGVAR_SIDE
{
    ConfigVarSideBoth,
    ConfigVarSideClientOnly,
    ConfigVarSideServerOnly
} CONFIGVAR_SIDE, *PCONFIGVAR_SIDE;

/// @brief The value of a configuration variable
typedef union CONFIGVAR_VALUE {
    BOOLEAN Boolean;
    UINT64 Int;
    DOUBLE Float;
    struct
    {
        CHAR Value[64];
    } String;
} CONFIGVAR_VALUE, *PCONFIGVAR_VALUE;

/// @brief A configuration variable
typedef struct CONFIGVAR
{
    CHAR Name[32];
    CONFIGVAR_TYPE Type;
    CONFIGVAR_VALUE Default;
    CONFIGVAR_VALUE Current;
    union {
        UINT8 Bitflags;
        struct
        {
            UINT8 Static : 1;
            UINT8 Changed : 1;
            UINT8 Side : 2;
            UINT8 Cheat : 1;
        };
    };
} CONFIGVAR, *PCONFIGVAR;

/// @brief Define a configuration variable
///
/// @param[in] Name The name of the variable
/// @param[in] DefaultValue A pointer to the value to set
/// @param[in] Type The type of the variable, determines the size read from Value
/// @param[in] Static Whether the variable can be changed after this function is called
/// @param[in] Side The side of the variable
/// @param[in] Cheat Whether changing the variable is cheating
extern VOID CfgDefineVariable(_In_ PCSTR Name, _In_ CONST PVOID DefaultValue, _In_ CONFIGVAR_TYPE Type,
                                  _In_ BOOLEAN Static, _In_ CONFIGVAR_SIDE Side, _In_ BOOLEAN Cheat);

/// @brief Get a configuration variable
///
/// @param Name The name of the variable
///
/// @return If Name exists, the variable it's tied to. Otherwise, NULL.
extern PCONFIGVAR CfgGetVariable(_In_ PCSTR Name);

#define CONFIGVAR_GET_BOOLEAN(Name) CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.Boolean : FALSE
#define CONFIGVAR_GET_INT(Name) CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.Int : 0
#define CONFIGVAR_GET_FLOAT(Name) CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.Float : 0.0
#define CONFIGVAR_GET_STRING(Name) CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.String.Value : NULL
