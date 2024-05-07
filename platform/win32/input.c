#include "common/common.h"

#ifdef PURPL_ENGINE
#include "engine/render/render.h"
#endif

#include "platform/input.h"

extern INT32 WindowWidth;
extern INT32 WindowHeight;

#define KEYBOARD_COUNT 255

INPUT_STATE InState;
static PAS_MUTEX Mutex;
BOOLEAN Keyboard[KEYBOARD_COUNT];

// Generated from this and manually tweaked (keys.txt came from the table in the virtual key documentation on MSDN):
//
// with open("c:/temp/keys.txt", mode="rb") as file:
//     content = bytes.decode(file.read(), encoding="utf-8")
//
// i = 0
// for row in content.split("\r\n"):
//     columns = [column.strip() for column in row.split(" \t")]
//     if columns[0] != "-":
//         if len(columns) == 3:
//             while i < int(columns[1], 16):
//                 print(f"InputKeyCount, // filler 0x{i:02X}")
//                 i += 1
//             if columns[2].startswith("IME") or columns[2].startswith("OEM"):
//                 print(f"InputKeyCount, // {columns[1]} {columns[2]}")
//                 i += 1
//             else:
//                 value_range = columns[1].split("-")
//                 if len(value_range) > 1:
//                     for i in range(int(value_range[0], 16), int(value_range[1], 16)):
//                         print(f"InputKeyCount, // {i} {columns[2]}")
//                         i += 1
//                 else:
//                     name = columns[0].replace("VK_", "").lower().capitalize()
//                     print(f"InputKey{name}, // {columns[1]} {columns[2]}")
//                     i += 1
//         elif len(columns) == 2:
//             character = chr(int(columns[0], 16))
//             while i < ord(character):
//                 print(f"InputKeyCount, // filler 0x{i:02X}")
//                 i += 1
//             print(f"InputKey{character}, // {columns[0]} {character}")
//             i += 1

static INPUT_KEY KeyLookupTable[KEYBOARD_COUNT] = {
    InputKeyCount,        // 0x01 Left mouse button
    InputKeyCount,        // 0x02 Right mouse button
    InputKeyCount,        // 0x03 Control-break processing
    InputKeyCount,        // 0x04 Middle mouse button
    InputKeyCount,        // 0x05 X1 mouse button
    InputKeyCount,        // 0x06 X2 mouse button
    InputKeyCount,        // filler 0x07
    InputKeyBackspace,    // 0x08 BACKSPACE key
    InputKeyTab,          // 0x09 TAB key
    InputKeyCount,        // filler 0x0A
    InputKeyCount,        // filler 0x0B
    InputKeyCount,        // 0x0C CLEAR key
    InputKeyEnter,        // 0x0D ENTER key
    InputKeyCount,        // filler 0x0E
    InputKeyCount,        // filler 0x0F
    InputKeyCount,        // 0x10 SHIFT key
    InputKeyCount,        // 0x11 CTRL key
    InputKeyCount,         // 0x12 ALT key
    InputKeyPauseBreak,   // 0x13 PAUSE key
    InputKeyCapsLock,     // 0x14 CAPS LOCK key
    InputKeyCount,        // 0x15 IME Kana mode
    InputKeyCount,        // 0x16 IME On
    InputKeyCount,        // 0x17 IME Junja mode
    InputKeyCount,        // 0x18 IME final mode
    InputKeyCount,        // 0x19 IME Hanja mode
    InputKeyCount,        // 0x19 IME Kanji mode
    InputKeyCount,        // 0x1A IME Off
    InputKeyEscape,       // 0x1B ESC key
    InputKeyCount,        // 0x1C IME convert
    InputKeyCount,        // 0x1D IME nonconvert
    InputKeyCount,        // 0x1E IME accept
    InputKeyCount,        // 0x1F IME mode change request
    InputKeySpace,        // 0x20 SPACEBAR
    InputKeyPageUp,       // 0x21 PAGE UP key
    InputKeyPageDown,     // 0x22 PAGE DOWN key
    InputKeyEnd,          // 0x23 END key
    InputKeyHome,         // 0x24 HOME key
    InputKeyLeft,         // 0x25 LEFT ARROW key
    InputKeyUp,           // 0x26 UP ARROW key
    InputKeyRight,        // 0x27 RIGHT ARROW key
    InputKeyDown,         // 0x28 DOWN ARROW key
    InputKeyCount,        // 0x29 SELECT key
    InputKeyCount,        // 0x2A PRINT key
    InputKeyCount,        // 0x2B EXECUTE key
    InputKeyPrintScreen,  // 0x2C PRINT SCREEN key
    InputKeyInsert,       // 0x2D INS key
    InputKeyDelete,       // 0x2E DEL key
    InputKeyCount,        // 0x2F HELP key
    InputKey0,            // 0x30 0
    InputKey1,            // 0x31 1
    InputKey2,            // 0x32 2
    InputKey3,            // 0x33 3
    InputKey4,            // 0x34 4
    InputKey5,            // 0x35 5
    InputKey6,            // 0x36 6
    InputKey7,            // 0x37 7
    InputKey8,            // 0x38 8
    InputKey9,            // 0x39 9
    InputKeyCount,        // filler 0x3A
    InputKeyCount,        // filler 0x3B
    InputKeyCount,        // filler 0x3C
    InputKeyCount,        // filler 0x3D
    InputKeyCount,        // filler 0x3E
    InputKeyCount,        // filler 0x3F
    InputKeyCount,        // filler 0x40
    InputKeyA,            // 0x41 A
    InputKeyB,            // 0x42 B
    InputKeyC,            // 0x43 C
    InputKeyD,            // 0x44 D
    InputKeyE,            // 0x45 E
    InputKeyF,            // 0x46 F
    InputKeyG,            // 0x47 G
    InputKeyH,            // 0x48 H
    InputKeyI,            // 0x49 I
    InputKeyJ,            // 0x4A J
    InputKeyK,            // 0x4B K
    InputKeyL,            // 0x4C L
    InputKeyM,            // 0x4D M
    InputKeyN,            // 0x4E N
    InputKeyO,            // 0x4F O
    InputKeyP,            // 0x50 P
    InputKeyQ,            // 0x51 Q
    InputKeyR,            // 0x52 R
    InputKeyS,            // 0x53 S
    InputKeyT,            // 0x54 T
    InputKeyU,            // 0x55 U
    InputKeyV,            // 0x56 V
    InputKeyW,            // 0x57 W
    InputKeyX,            // 0x58 X
    InputKeyY,            // 0x59 Y
    InputKeyZ,            // 0x5A Z
    InputKeyLeftSuper,    // 0x5B Left Windows key
    InputKeyRightSuper,   // 0x5C Right Windows key
    InputKeyCount,        // 0x5D Applications key
    InputKeyCount,        // filler 0x5E
    InputKeyCount,        // 0x5F Computer Sleep key
    InputKeyNumpad0,      // 0x60 Numeric keypad 0 key
    InputKeyNumpad1,      // 0x61 Numeric keypad 1 key
    InputKeyNumpad2,      // 0x62 Numeric keypad 2 key
    InputKeyNumpad3,      // 0x63 Numeric keypad 3 key
    InputKeyNumpad4,      // 0x64 Numeric keypad 4 key
    InputKeyNumpad5,      // 0x65 Numeric keypad 5 key
    InputKeyNumpad6,      // 0x66 Numeric keypad 6 key
    InputKeyNumpad7,      // 0x67 Numeric keypad 7 key
    InputKeyNumpad8,      // 0x68 Numeric keypad 8 key
    InputKeyNumpad9,      // 0x69 Numeric keypad 9 key
    InputKeyMultiply,     // 0x6A Multiply key
    InputKeyAdd,          // 0x6B Add key
    InputKeyCount,        // 0x6C Separator key
    InputKeySubtract,     // 0x6D Subtract key
    InputKeyDecimal,      // 0x6E Decimal key
    InputKeyDivide,       // 0x6F Divide key
    InputKeyF1,           // 0x70 F1 key
    InputKeyF2,           // 0x71 F2 key
    InputKeyF3,           // 0x72 F3 key
    InputKeyF4,           // 0x73 F4 key
    InputKeyF5,           // 0x74 F5 key
    InputKeyF6,           // 0x75 F6 key
    InputKeyF7,           // 0x76 F7 key
    InputKeyF8,           // 0x77 F8 key
    InputKeyF9,           // 0x78 F9 key
    InputKeyF10,          // 0x79 F10 key
    InputKeyF11,          // 0x7A F11 key
    InputKeyF12,          // 0x7B F12 key
    InputKeyCount,        // 0x7C F13 key
    InputKeyCount,        // 0x7D F14 key
    InputKeyCount,        // 0x7E F15 key
    InputKeyCount,        // 0x7F F16 key
    InputKeyCount,        // 0x80 F17 key
    InputKeyCount,        // 0x81 F18 key
    InputKeyCount,        // 0x82 F19 key
    InputKeyCount,        // 0x83 F20 key
    InputKeyCount,        // 0x84 F21 key
    InputKeyCount,        // 0x85 F22 key
    InputKeyCount,        // 0x86 F23 key
    InputKeyCount,        // 0x87 F24 key
    InputKeyCount,        // filler 0x88
    InputKeyCount,        // filler 0x89
    InputKeyCount,        // filler 0x8A
    InputKeyCount,        // filler 0x8B
    InputKeyCount,        // filler 0x8C
    InputKeyCount,        // filler 0x8D
    InputKeyCount,        // filler 0x8E
    InputKeyCount,        // filler 0x8F
    InputKeyNumLock,      // 0x90 NUM LOCK key
    InputKeyScrollLock,   // 0x91 SCROLL LOCK key
    InputKeyCount,        // filler 0x92
    InputKeyCount,        // filler 0x93
    InputKeyCount,        // filler 0x94
    InputKeyCount,        // filler 0x95
    InputKeyCount,        // filler 0x96
    InputKeyCount,        // filler 0x97
    InputKeyCount,        // filler 0x98
    InputKeyCount,        // filler 0x99
    InputKeyCount,        // filler 0x9A
    InputKeyCount,        // filler 0x9B
    InputKeyCount,        // filler 0x9C
    InputKeyCount,        // filler 0x9D
    InputKeyCount,        // filler 0x9E
    InputKeyCount,        // filler 0x9F
    InputKeyLeftShift,    // 0xA0 Left SHIFT key
    InputKeyRightShift,   // 0xA1 Right SHIFT key
    InputKeyLeftControl,  // 0xA2 Left CONTROL key
    InputKeyRightControl, // 0xA3 Right CONTROL key
    InputKeyLeftAlt,      // 0xA4 Left ALT key
    InputKeyLeftAlt,      // 0xA5 Right ALT key
    InputKeyCount,        // 0xA6 Browser Back key
    InputKeyCount,        // 0xA7 Browser Forward key
    InputKeyCount,        // 0xA8 Browser Refresh key
    InputKeyCount,        // 0xA9 Browser Stop key
    InputKeyCount,        // 0xAA Browser Search key
    InputKeyCount,        // 0xAB Browser Favorites key
    InputKeyCount,        // 0xAC Browser Start and Home key
    InputKeyCount,        // 0xAD Volume Mute key
    InputKeyCount,        // 0xAE Volume Down key
    InputKeyCount,        // 0xAF Volume Up key
    InputKeyCount,        // 0xB0 Next Track key
    InputKeyCount,        // 0xB1 Previous Track key
    InputKeyCount,        // 0xB2 Stop Media key
    InputKeyCount,        // 0xB3 Play/Pause Media key
    InputKeyCount,        // 0xB4 Start Mail key
    InputKeyCount,        // 0xB5 Select Media key
    InputKeyCount,        // 0xB6 Start Application 1 key
    InputKeyCount,        // 0xB7 Start Application 2 key
    InputKeyCount,        // filler 0xB8
    InputKeyCount,        // filler 0xB9
    InputKeySemicolon, // 0xBA Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard,
                       // the
                       // ;: key
    InputKeyEquals,    // 0xBB For any country/region, the + key
    InputKeyComma,     // 0xBC For any country/region, the , key
    InputKeyMinus,     // 0xBD For any country/region, the - key
    InputKeyPeriod,    // 0xBE For any country/region, the . key
    InputKeySlash, // 0xBF Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the
                   // /? key
    InputKeyGrave, // 0xC0 Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the
                   // `~ key
    InputKeyCount, // filler 0xC1
    InputKeyCount, // filler 0xC2
    InputKeyCount, // filler 0xC3
    InputKeyCount, // filler 0xC4
    InputKeyCount, // filler 0xC5
    InputKeyCount, // filler 0xC6
    InputKeyCount, // filler 0xC7
    InputKeyCount, // filler 0xC8
    InputKeyCount, // filler 0xC9
    InputKeyCount, // filler 0xCA
    InputKeyCount, // filler 0xCB
    InputKeyCount, // filler 0xCC
    InputKeyCount, // filler 0xCD
    InputKeyCount, // filler 0xCE
    InputKeyCount, // filler 0xCF
    InputKeyCount, // filler 0xD0
    InputKeyCount, // filler 0xD1
    InputKeyCount, // filler 0xD2
    InputKeyCount, // filler 0xD3
    InputKeyCount, // filler 0xD4
    InputKeyCount, // filler 0xD5
    InputKeyCount, // filler 0xD6
    InputKeyCount, // filler 0xD7
    InputKeyCount, // filler 0xD8
    InputKeyCount, // filler 0xD9
    InputKeyCount, // filler 0xDA
    InputKeyLeftBracket, // 0xDB Used for miscellaneous characters; it can vary by keyboard. For the US standard
                         // keyboard, the
                         // [{ key
    InputKeyBackslash, // 0xDC Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard,
                       // the
                       // \\| key
    InputKeyRightBracket, // 0xDD Used for miscellaneous characters; it can vary by keyboard. For the US standard
                          // keyboard, the
                          // ]} key
    InputKeyApostrophe,   // 0xDE Used for miscellaneous characters; it can vary by keyboard. For the US standard
                          // keyboard, the
                          // '" key
    InputKeyCount,        // 0xDF Used for miscellaneous characters; it can vary by keyboard.
    InputKeyCount,        // filler 0xE0
    InputKeyCount,        // filler 0xE1
    InputKeyBackslash,    // 0xE2 The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard
    InputKeyCount,        // filler 0xE3
    InputKeyCount,        // filler 0xE4
    InputKeyCount,        // 0xE5 IME PROCESS key
    InputKeyCount,        // filler 0xE6
    InputKeyCount, // 0xE7 Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low
                   // word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see
                   // Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
    InputKeyCount, // filler 0xE8
    InputKeyCount, // filler 0xE9
    InputKeyCount, // filler 0xEA
    InputKeyCount, // filler 0xEB
    InputKeyCount, // filler 0xEC
    InputKeyCount, // filler 0xED
    InputKeyCount, // filler 0xEE
    InputKeyCount, // filler 0xEF
    InputKeyCount, // filler 0xF0
    InputKeyCount, // filler 0xF1
    InputKeyCount, // filler 0xF2
    InputKeyCount, // filler 0xF3
    InputKeyCount, // filler 0xF4
    InputKeyCount, // filler 0xF5
    InputKeyCount, // 0xF6 Attn key
    InputKeyCount, // 0xF7 CrSel key
    InputKeyCount, // 0xF8 ExSel key
    InputKeyCount, // 0xF9 Erase EOF key
    InputKeyCount, // 0xFA Play key
    InputKeyCount, // 0xFB Zoom key
    InputKeyCount, // 0xFC Reserved
    InputKeyCount, // 0xFD PA1 key
    InputKeyCount, // 0xFE Clear key
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

    // TODO: handle key bindings and switching between controller/keyboard
    InState.LeftAxis[1] = 0.0f;
    if (INPUT_GET_KEY(W))
    {
        InState.LeftAxis[1] += 1.0f;
    }
    if (INPUT_GET_KEY(S))
    {
        InState.LeftAxis[1] += -1.0f;
    }

    InState.LeftAxis[0] = 0.0f;
    if (INPUT_GET_KEY(A))
    {
        InState.LeftAxis[0] += -1.0f;
    }
    if (INPUT_GET_KEY(D))
    {
        InState.LeftAxis[0] += 1.0f;
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

static BOOLEAN CursorLocked = TRUE;

VOID InLockCursor(_In_ BOOLEAN Locked)
{
    CursorLocked = TRUE;
}

static VOID LockCursor(VOID)
{
    RECT WindowRect = {0};
    GetWindowRect(VidGetObject(), &WindowRect);
    SetCursorPos(WindowRect.left + (WindowRect.right - WindowRect.left) / 2,
                 WindowRect.top + (WindowRect.bottom - WindowRect.top) / 2);
    ShowCursor(FALSE);
    RECT ClientRect = {0};
    GetClientRect(VidGetObject(), &ClientRect);
    ClipCursor(&ClientRect);
}

static VOID UnlockCursor(VOID)
{
    ShowCursor(TRUE);
    ClipCursor(NULL);
}

VOID InUpdateState(VOID)
{
    if (AsLockMutex(Mutex, TRUE))
    {
        InState.Type = InputTypeKeyboardAndMouse;

        UpdateKeyboard();
        UpdateMouse();
        UpdateController();

        if (CursorLocked && VidFocused())
        {
            LockCursor();
        }
        else
        {
            UnlockCursor();
        }

        AsUnlockMutex(Mutex);
    }
}

VOID InShutdown(VOID)
{
    UnlockCursor();
    AsDestroyMutex(Mutex);
}
