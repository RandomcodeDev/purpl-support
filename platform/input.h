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

/// @brief Keys, based on position on QWERTY keyboard
typedef enum INPUT_KEY
{
    // row 1

    InputKeyEscape = 0,
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
    InputKeyDecimal,

    InputKeyCount
} INPUT_KEY, *PINPUT_KEY;

/// @brief Get the current state of a key
#define INPUT_GET_KEY(Key) (InState.Keyboard[InputKey##Key])

/// @brief Mouse buttons
typedef enum INPUT_MOUSE_BUTTON
{
    InputMouseButtonLeft,
    InputMouseButtonMiddle,
    InputMouseButtonRight,
    InputMouseButton4,
    InputMouseButton5,
    InputMouseButtonCount
} INPUT_MOUSE_BUTTON, *PINPUT_MOUSE_BUTTON;

/// @brief Get the current state of a mouse button
#define INPUT_GET_MOUSE_BUTTOM(Button) (InState.MouseButtons[InputMouseButton##Button])

/// @brief Controller buttons, names/positions based on Xbox controller
typedef enum INPUT_CONTROLLER_BUTTON
{
    InputControllerButtonA,
    InputControllerButtonB,
    InputControllerButtonX,
    InputControllerButtonY,

    InputControllerButtonStart,
    InputControllerButtonSelect,

    InputControllerButtonLeftStick,
    InputControllerButtonRightStick,

    InputControllerButtonUp,
    InputControllerButtonDown,
    InputControllerButtonLeft,
    InputControllerButtonRight,

    InputControllerButtonLeftTrigger,
    InputControllerButtonRightTrigger,
    InputControllerButtonLeftShoulder,
    InputControllerButtonRightShoulder,

    InputControllerButtonCount
} INPUT_CONTROLLER_BUTTON, *PINPUT_CONTROLLER_BUTTON;

/// @brief Get the current state of a controller button
#define INPUT_GET_CONTROLLER_BUTTON(Button) (InState.Keyboard[InputKey##Key])

/// @brief Input state
typedef struct INPUT_STATE
{
    INPUT_TYPE Type;

    BOOLEAN Keyboard[InputKeyCount];
    BOOLEAN MouseButtons[InputMouseButtonCount];
    BOOLEAN ControllerButtons[InputControllerButtonCount];

    UINT32 ControllerIndex;

    // 0 - RdrGetWidth()/RdrGetHeight()
    ivec2 MousePosition;

    vec2 LeftAxis;
    vec2 RightAxis;
    vec2 DpadAxis;
} INPUT_STATE, *PINPUT_STATE;

BEGIN_EXTERN_C

/// @brief Input state
extern INPUT_STATE InState;

/// @brief Initialize input
extern VOID InInitialize(VOID);

/// @brief Toggle whether the cursor is locked to the center of the screen and invisible
extern VOID InLockCursor(_In_ BOOLEAN Locked);

/// @brief Update input state
extern VOID InUpdateState(VOID);

/// @brief Shut down input
extern VOID InShutdown(VOID);

END_EXTERN_C
