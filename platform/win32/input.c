#include "common/common.h"

#ifdef PURPL_ENGINE
#include "engine/render/render.h"
#endif

#include "platform/input.h"

extern INT32 WindowX;
extern INT32 WindowY;
extern INT32 WindowWidth;
extern INT32 WindowHeight;

#define KEYBOARD_COUNT 256

INPUT_STATE InState;
static PAS_MUTEX Mutex;
BOOLEAN Keyboard[KEYBOARD_COUNT];

static INPUT_KEY KeyLookupTable[KEYBOARD_COUNT] = {
    InputKeyCount,        // Left mouse button
    InputKeyCount,        // Right mouse button
    InputKeyCount,        // Control-break processing
    InputKeyCount,        // Middle mouse button
    InputKeyCount,        // X1 mouse button
    InputKeyCount,        // X2 mouse button
    InputKeyBackspace,    // BACKSPACE key
    InputKeyTab,          // TAB key
    InputKeyCount,        // CLEAR key
    InputKeyCount,        // ENTER key
    InputKeyCount,        // SHIFT key
    InputKeyCount,        // CTRL key
    InputKeyCount,        // ALT key
    InputKeyPauseBreak,   // PAUSE key
    InputKeyCapsLock,     // CAPS LOCK key
    InputKeyCount,        // IME Kana mode
    InputKeyCount,        // IME Hangul mode
    InputKeyCount,        // IME On
    InputKeyCount,        // IME Junja mode
    InputKeyCount,        // IME final mode
    InputKeyCount,        // IME Hanja mode
    InputKeyCount,        // IME Kanji mode
    InputKeyCount,        // IME Off
    InputKeyEscape,       // ESC key
    InputKeyCount,        // IME convert
    InputKeyCount,        // IME nonconvert
    InputKeyCount,        // IME accept
    InputKeyCount,        // IME mode change request
    InputKeySpace,        // SPACEBAR
    InputKeyPageUp,       // PAGE UP key
    InputKeyPageDown,     // PAGE DOWN key
    InputKeyEnd,          // END key
    InputKeyHome,         // HOME key
    InputKeyLeft,         // LEFT ARROW key
    InputKeyUp,           // UP ARROW key
    InputKeyRight,        // RIGHT ARROW key
    InputKeyDown,         // DOWN ARROW key
    InputKeyCount,        // SELECT key
    InputKeyCount,        // PRINT key
    InputKeyCount,        // EXECUTE key
    InputKeyPrintScreen,  // PRINT SCREEN key
    InputKeyInsert,       // INS key
    InputKeyDelete,       // DEL key
    InputKeyCount,        // HELP key
    InputKey0,            // 0
    InputKey1,            // 1
    InputKey2,            // 2
    InputKey3,            // 3
    InputKey4,            // 4
    InputKey5,            // 5
    InputKey6,            // 6
    InputKey7,            // 7
    InputKey8,            // 8
    InputKey9,            // 9
    InputKeyA,            // A
    InputKeyB,            // B
    InputKeyC,            // C
    InputKeyD,            // D
    InputKeyE,            // E
    InputKeyF,            // F
    InputKeyG,            // G
    InputKeyH,            // H
    InputKeyI,            // I
    InputKeyJ,            // J
    InputKeyK,            // K
    InputKeyL,            // L
    InputKeyM,            // M
    InputKeyN,            // N
    InputKeyO,            // O
    InputKeyP,            // P
    InputKeyQ,            // Q
    InputKeyR,            // R
    InputKeyS,            // S
    InputKeyT,            // T
    InputKeyU,            // U
    InputKeyV,            // V
    InputKeyW,            // W
    InputKeyX,            // X
    InputKeyY,            // Y
    InputKeyZ,            // Z
    InputKeyLeftSuper,    // Left Windows key
    InputKeyRightSuper,   // Right Windows key
    InputKeyMenu,         // Applications key
    InputKeyCount,        // Computer Sleep key
    InputKeyNumpad0,      // Numeric keypad 0 key
    InputKeyNumpad1,      // Numeric keypad 1 key
    InputKeyNumpad2,      // Numeric keypad 2 key
    InputKeyNumpad3,      // Numeric keypad 3 key
    InputKeyNumpad4,      // Numeric keypad 4 key
    InputKeyNumpad5,      // Numeric keypad 5 key
    InputKeyNumpad6,      // Numeric keypad 6 key
    InputKeyNumpad7,      // Numeric keypad 7 key
    InputKeyNumpad8,      // Numeric keypad 8 key
    InputKeyNumpad9,      // Numeric keypad 9 key
    InputKeyMultiply,     // Multiply key
    InputKeyAdd,          // Add key
    InputKeyCount,        // Separator key
    InputKeySubtract,     // Subtract key
    InputKeyNumpadPeriod, // Decimal key
    InputKeyDivide,       // Divide key
    InputKeyF1,           // F1 key
    InputKeyF2,           // F2 key
    InputKeyF3,           // F3 key
    InputKeyF4,           // F4 key
    InputKeyF5,           // F5 key
    InputKeyF6,           // F6 key
    InputKeyF7,           // F7 key
    InputKeyF8,           // F8 key
    InputKeyF9,           // F9 key
    InputKeyF10,          // F10 key
    InputKeyF11,          // F11 key
    InputKeyF12,          // F12 key
    InputKeyCount,        // F13 key
    InputKeyCount,        // F14 key
    InputKeyCount,        // F15 key
    InputKeyCount,        // F16 key
    InputKeyCount,        // F17 key
    InputKeyCount,        // F18 key
    InputKeyCount,        // F19 key
    InputKeyCount,        // F20 key
    InputKeyCount,        // F21 key
    InputKeyCount,        // F22 key
    InputKeyCount,        // F23 key
    InputKeyCount,        // F24 key
    InputKeyNumLock,      // NUM LOCK key
    InputKeyScrollLock,   // SCROLL LOCK key
    InputKeyLeftShift,    // Left SHIFT key
    InputKeyRightShift,   // Right SHIFT key
    InputKeyLeftControl,  // Left CONTROL key
    InputKeyRightControl, // Right CONTROL key
    InputKeyLeftAlt,      // Left ALT key
    InputKeyRightAlt,     // Right ALT key
    InputKeyCount,        // Browser Back key
    InputKeyCount,        // Browser Forward key
    InputKeyCount,        // Browser Refresh key
    InputKeyCount,        // Browser Stop key
    InputKeyCount,        // Browser Search key
    InputKeyCount,        // Browser Favorites key
    InputKeyCount,        // Browser Start and Home key
    InputKeyCount,        // Volume Mute key
    InputKeyCount,        // Volume Down key
    InputKeyCount,        // Volume Up key
    InputKeyCount,        // Next Track key
    InputKeyCount,        // Previous Track key
    InputKeyCount,        // Stop Media key
    InputKeyCount,        // Play/Pause Media key
    InputKeyCount,        // Start Mail key
    InputKeyCount,        // Select Media key
    InputKeyCount,        // Start Application 1 key
    InputKeyCount,        // Start Application 2 key
    InputKeySemicolon, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the
                       // ;: key
    InputKeyEquals,    // For any country/region, the + key
    InputKeyComma,     // For any country/region, the , key
    InputKeyMinus,     // For any country/region, the - key
    InputKeyPeriod,    // For any country/region, the . key
    InputKeySlash, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the /?
                   // key
    InputKeyGrave, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the `~
                   // key
    InputKeyLeftBracket, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard,
                         // the [{ key
    InputKeyBackslash, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the
                       // \\| key
    InputKeyRightBracket, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard,
                          // the ]} key
    InputKeyApostrophe, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the
                        // '" key
    InputKeyCount,      // Used for miscellaneous characters; it can vary by keyboard.
    InputKeyCount,      // The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard
    InputKeyCount,      // IME PROCESS key
    InputKeyCount, // Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of
                   // a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark
                   // in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
    InputKeyCount, // Attn key
    InputKeyCount, // CrSel key
    InputKeyCount, // ExSel key
    InputKeyCount, // Erase EOF key
    InputKeyCount, // Play key
    InputKeyCount, // Zoom key
    InputKeyCount, // Reserved
    InputKeyCount, // PA1 key
    InputKeyCount, // Clear key
};

static VOID UpdateKeyboard(VOID)
{
    for (UINT32 Key = 0; Key < KEYBOARD_COUNT; Key++)
    {
        INPUT_KEY Mapped = KeyLookupTable[Key];
        if (Mapped >= 0 && Mapped < InputKeyCount)
        {
            InState.Keyboard[Mapped] = Keyboard[Key];
        }
    }

    // TODO: handle keybindings
    if (INPUT_GET_KEY(W) && INPUT_GET_KEY(S))
    {
        InState.LeftAxis[1] = 0.0f;
    }
    else if (INPUT_GET_KEY(W))
    {
        InState.LeftAxis[1] = 1.0f;
    }
    else if (INPUT_GET_KEY(S))
    {
        InState.LeftAxis[1] = -1.0f;
    }

    if (INPUT_GET_KEY(A) && INPUT_GET_KEY(D))
    {
        InState.LeftAxis[0] = 0.0f;
    }
    else if (INPUT_GET_KEY(A))
    {
        InState.LeftAxis[0] = 1.0f;
    }
    else if (INPUT_GET_KEY(D))
    {
        InState.LeftAxis[0] = -1.0f;
    }
}

static VOID UpdateMouse(VOID)
{
    InState.MouseButtons[InputMouseButtonLeft] = Keyboard[VK_LBUTTON];
    InState.MouseButtons[InputMouseButtonMiddle] = Keyboard[VK_MBUTTON];
    InState.MouseButtons[InputMouseButtonRight] = Keyboard[VK_RBUTTON];
    InState.MouseButtons[InputMouseButton4] = Keyboard[VK_XBUTTON1];
    InState.MouseButtons[InputMouseButton5] = Keyboard[VK_XBUTTON2];

    POINT CursorPosition;
    GetCursorPos(&CursorPosition);
    ScreenToClient((HWND)VidGetObject(), &CursorPosition);

    FLOAT Scale = (FLOAT)CONFIGVAR_GET_FLOAT_EX("rdr_scale", 1.0);

    INT32 CenterX = WindowWidth / 2;
    INT32 CenterY = WindowHeight / 2;

    InState.MousePosition[0] = CursorPosition.x * Scale;
    InState.MousePosition[1] = CursorPosition.y * Scale;

    InState.RightAxis[0] = (InState.MousePosition[0] - CenterX * Scale) / RdrGetWidth();
    InState.RightAxis[1] = (InState.MousePosition[1] - CenterY * Scale) / RdrGetHeight();
}

static VOID UpdateController(VOID)
{
}

VOID InInitialize(VOID)
{
    Mutex = AsCreateMutex();
}

VOID InUpdateState(VOID)
{
    AsLockMutex(Mutex, TRUE);

    InState.Type = InputTypeKeyboardAndMouse;

    UpdateKeyboard();
    UpdateMouse();
    UpdateController();

    printf("\r%-3.3f %-3.3f %-3.3f %-3.3f", InState.LeftAxis[0], InState.LeftAxis[1], InState.RightAxis[0],
           InState.RightAxis[1]);

    AsUnlockMutex(Mutex);
}

VOID InShutdown(VOID)
{
    AsDestroyMutex(Mutex);
}
