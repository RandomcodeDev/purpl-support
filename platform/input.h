/// @file input.h
///
/// @brief This file defines the platform input API
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "purpl/purpl.h"

/// @brief Input type
typedef enum INPUT_TYPE
{
    InputTypeKeyboardAndMouse,
    InputTypeController,
    // TODO: vr etc
    InputTypeCount
} INPUT_TYPE, *PINPUT_TYPE;

typedef enum INPUT_KEY INPUT_KEY, *PINPUT_KEY;
typedef enum INPUT_MOUSE_BUTTON INPUT_MOUSE_BUTTON, *PINPUT_MOUSE_BUTTON;
typedef enum INPUT_CONTROLLER_BUTTON INPUT_CONTROLLER_BUTTON, *PINPUT_CONTROLLER_BUTTON;

BEGIN_EXTERN_C

/// @brief Get whether a key is down or not
///
/// @param[in] Key The key to get the state of
extern BOOLEAN InGetKeyState(_In_ INPUT_KEY Key);

END_EXTERN_C

/// @brief Keys, based on position on QWERTY keyboard
typedef enum INPUT_KEY
{
    // row 1

    InputKeyEscape,
    InputKeyF1,
    InputKeyF2,
    InputKeyF3,
    InputKeyF4,
    InputKeyF5,
    InputKeyF6,
    InputKeyF7,
    InputKeyF8,
    InputKeyF9,
    InputKeyF10,
    InputKeyF11,
    InputKeyF12,
    InputKeyPrintScreen,
    InputKeyScrollLock,
    InputKeyPauseBreak,

    // row 2

    InputKeyGrave,
    InputKey1,
    InputKey2,
    InputKey3,
    InputKey4,
    InputKey5,
    InputKey6,
    InputKey7,
    InputKey8,
    InputKey9,
    InputKey0,
    InputKeyMinus,
    InputKeyEquals,
    InputKeyBackspace,
    InputKeyInsert,
    InputKeyHome,
    InputKeyPageUp,
    InputKeyNumLock,
    InputKeyDivide,
    InputKeyMultiply,
    InputKeySubtract,

    // row 3

    InputKeyTab,
    InputKeyQ,
    InputKeyW,
    InputKeyE,
    InputKeyR,
    InputKeyT,
    InputKeyY,
    InputKeyU,
    InputKeyI,
    InputKeyO,
    InputKeyP,
    InputKeyLeftBracket,
    InputKeyRightBracket,
    InputKeyBackslash,
    InputKeyDelete,
    InputKeyEnd,
    InputKeyPageDown,
    InputKeyNumpad7,
    InputKeyNumpad8,
    InputKeyNumpad9,
    InputKeyAdd,

    // row 4

    InputKeyCapsLock,
    InputKeyA,
    InputKeyS,
    InputKeyD,
    InputKeyF,
    InputKeyG,
    InputKeyH,
    InputKeyJ,
    InputKeyK,
    InputKeyL,
    InputKeySemicolon,
    InputKeyApostrophe,
    InputKeyEnter,
    InputKeyNumpad4,
    InputKeyNumpad5,
    InputKeyNumpad6,

    // row 5
    InputKeyLeftShift,
    InputKeyZ,
    InputKeyX,
    InputKeyC,
    InputKeyV,
    InputKeyB,
    InputKeyN,
    InputKeyM,
    InputKeyComma,
    InputKeyPeriod,
    InputKeySlash,
    InputKeyRightShift,
    InputKeyUp,
    InputKeyNumpad1,
    InputKeyNumpad2,
    InputKeyNumpad3,
    InputKeyNumpadEnter,

    // row 6

    InputKeyLeftControl,
    InputKeyLeftSuper,
    InputKeyLeftAlt,
    InputKeySpace,
    InputKeyRightAlt,
    InputKeyRightSuper,
    InputKeyMenu,
    InputKeyRightControl,
    InputKeyLeft,
    InputKeyDown,
    InputKeyRight,
    InputKeyNumpad0,
    InputKeyNumpadPeriod,
} INPUT_KEY, *PINPUT_KEY;
