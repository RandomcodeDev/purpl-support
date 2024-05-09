/// @file configvar.h
///
/// @brief This file defines the interface for configuration variables
///
/// @copyright (c) Randomcode Developers 2024

#pragma once

#include "common/alloc.h"
#include "common/common.h"

/// @brief Data type of a variable
PURPL_MAKE_TAG(enum, CONFIGVAR_TYPE,
               {ConfigVarTypeBoolean, // BOOLEAN
                ConfigVarTypeInteger, // INT32
                ConfigVarTypeFloat,   // FLOAT
                ConfigVarTypeString,  // String, up to 64 characters including NUL
                ConfigVarTypeCount})

/// @brief The side of a variable
PURPL_MAKE_TAG(enum, CONFIGVAR_SIDE, {ConfigVarSideBoth, ConfigVarSideClientOnly, ConfigVarSideServerOnly})

/// @brief The value of a configuration variable
PURPL_MAKE_TAG(union, CONFIGVAR_VALUE, {
    BOOLEAN Boolean;
    INT64 Int;
    DOUBLE Float;
    CHAR String[64];
})

/// @brief A configuration variable. Do not modify the fields directly.
PURPL_MAKE_TAG(struct, CONFIGVAR, {
    CHAR Name[32];
    CONFIGVAR_TYPE Type;
    CONFIGVAR_VALUE Default;
    CONFIGVAR_VALUE Current;
    CONFIGVAR_SIDE Side;
    union {
        UINT8 Bitflags;
        struct
        {
            UINT8 Changed : 1;
            UINT8 Static : 1;
            UINT8 Cheat : 1;
            UINT8 Internal : 1;
        };
    };
})

/// @brief Define a configuration variable. Should be called before CmnInitialize if you want the variable to be parsed
/// from the command line arguments.
///
/// @param[in] Name The name of the variable
/// @param[in] DefaultValue A pointer to the value to set
/// @param[in] Type The type of the variable, determines the size read from Value
/// @param[in] Static Whether the variable can be changed after this function is called
/// @param[in] Side The side of the variable
/// @param[in] Cheat Whether changing the variable is cheating
/// @param[in] Internal Whether to allow the user to change this variable
extern VOID CfgDefineVariable(_In_z_ PCSTR Name, _In_ CONST PVOID DefaultValue, _In_ CONFIGVAR_TYPE Type,
                              _In_ BOOLEAN Static, _In_ CONFIGVAR_SIDE Side, _In_ BOOLEAN Cheat, _In_ BOOLEAN Internal);

#define CONFIGVAR_DEFINE_BOOLEAN(Name, DefaultValue, Static, Side, Cheat, Internal)                                    \
    {                                                                                                                  \
        BOOLEAN DefaultValue_ = (BOOLEAN)(DefaultValue);                                                               \
        CfgDefineVariable((Name), &DefaultValue_, ConfigVarTypeBoolean, (Static), (Side), (Cheat), (Internal));        \
    }
#define CONFIGVAR_DEFINE_INT(Name, DefaultValue, Static, Side, Cheat, Internal)                                        \
    {                                                                                                                  \
        INT64 DefaultValue_ = (INT64)(DefaultValue);                                                                   \
        CfgDefineVariable((Name), &DefaultValue_, ConfigVarTypeInteger, (Static), (Side), (Cheat), (Internal));        \
    }
#define CONFIGVAR_DEFINE_FLOAT(Name, DefaultValue, Static, Side, Cheat, Internal)                                      \
    {                                                                                                                  \
        DOUBLE DefaultValue_ = (DOUBLE)(DefaultValue);                                                                 \
        CfgDefineVariable((Name), &DefaultValue_, ConfigVarTypeFloat, (Static), (Side), (Cheat), (Internal));          \
    }
#define CONFIGVAR_DEFINE_STRING(Name, DefaultValue, Static, Side, Cheat, Internal)                                     \
    {                                                                                                                  \
        CfgDefineVariable((Name), (DefaultValue), ConfigVarTypeString, (Static), (Side), (Cheat), (Internal));         \
    }

/// @brief Get a configuration variable
///
/// @param[in] Name The name of the variable
///
/// @return If Name exists, the variable it's tied to. Otherwise, NULL.
extern PCONFIGVAR CfgGetVariable(_In_z_ PCSTR Name);

#define CONFIGVAR_HAS_CHANGED(Name) (CfgGetVariable(Name) ? CfgGetVariable(Name)->Changed : FALSE)
#define CONFIGVAR_CLEAR_CHANGED(Name) (CfgGetVariable(Name) ? CfgGetVariable(Name)->Changed = FALSE : 0)

#define CONFIGVAR_GET_TYPE(Name) (CfgGetVariable(Name) ? CfgGetVariable(Name)->Type : 0)

#define CONFIGVAR_GET_BOOLEAN_EX(Name, DefaultValue)                                                                   \
    (CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.Boolean : (DefaultValue))
#define CONFIGVAR_GET_INT_EX(Name, DefaultValue)                                                                       \
    (CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.Int : (DefaultValue))
#define CONFIGVAR_GET_FLOAT_EX(Name, DefaultValue)                                                                     \
    (CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.Float : (DefaultValue))
#define CONFIGVAR_GET_STRING_EX(Name, DefaultValue)                                                                    \
    (CfgGetVariable(Name) ? CfgGetVariable(Name)->Current.String.Value : (DefaultValue))

#define CONFIGVAR_GET_BOOLEAN(Name) CONFIGVAR_GET_BOOLEAN_EX(Name, (BOOLEAN)FALSE)
#define CONFIGVAR_GET_INT(Name) CONFIGVAR_GET_INT_EX(Name, 0)
#define CONFIGVAR_GET_FLOAT(Name) CONFIGVAR_GET_FLOAT_EX(Name, 0.0)
#define CONFIGVAR_GET_STRING(Name) CONFIGVAR_GET_STRING_EX(Name, NULL)

/// @brief Set a configuration variable
///
/// @param[in] Name The name of the variable
/// @param[in] Value The value of the variable
extern VOID CfgSetVariable(_In_z_ PCSTR Name, _In_ PVOID Value);
#define CONFIGVAR_SET_BOOLEAN(Name, Value)                                                                             \
    {                                                                                                                  \
        BOOLEAN Value_ = (BOOLEAN)(Value);                                                                             \
        CfgSetVariable((Name), &Value_);                                                                               \
    }
#define CONFIGVAR_SET_INT(Name, Value)                                                                                 \
    {                                                                                                                  \
        INT64 Value_ = (INT64)(Value);                                                                                 \
        CfgSetVariable((Name), &Value_);                                                                               \
    }
#define CONFIGVAR_SET_FLOAT(Name, Value)                                                                               \
    {                                                                                                                  \
        DOUBLE Value_ = (DOUBLE)(Value);                                                                               \
        CfgSetVariable((Name), &Value_);                                                                               \
    }
#define CONFIGVAR_SET_STRING(Name, Value)                                                                              \
    {                                                                                                                  \
        CfgSetVariable((Name), (Value));                                                                               \
    }
