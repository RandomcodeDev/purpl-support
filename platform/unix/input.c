#include "common/common.h"

#ifdef PURPL_ENGINE
#include "engine/render/render.h"
#endif

#include "GLFW/glfw3.h"

#include "platform/input.h"

INPUT_STATE InState;

INPUT_KEY KeyLookupTable[GLFW_KEY_LAST + 1] = {
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeySpace,
    InputKeyApostrophe,
    InputKeyComma,
    InputKeyMinus,
    InputKeyPeriod,
    InputKeySlash,
    InputKey0,
    InputKey1,
    InputKey2,
    InputKey3,
    InputKey4,
    InputKey5,
    InputKey6,
    InputKey7,
    InputKey8,
    InputKey9,
    InputKeySemicolon,
    InputKeyEquals,
    InputKeyA,
    InputKeyB,
    InputKeyC,
    InputKeyD,
    InputKeyE,
    InputKeyF,
    InputKeyG,
    InputKeyH,
    InputKeyI,
    InputKeyJ,
    InputKeyK,
    InputKeyL,
    InputKeyM,
    InputKeyN,
    InputKeyO,
    InputKeyP,
    InputKeyQ,
    InputKeyR,
    InputKeyS,
    InputKeyT,
    InputKeyU,
    InputKeyV,
    InputKeyW,
    InputKeyX,
    InputKeyY,
    InputKeyZ,
    InputKeyLeftBracket,
    InputKeyBackslash,
    InputKeyRightBracket,
    InputKeyGrave,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyEscape,
    InputKeyEnter,
    InputKeyTab,
    InputKeyBackspace,
    InputKeyInsert,
    InputKeyDelete,
    InputKeyRight,
    InputKeyLeft,
    InputKeyDown,
    InputKeyUp,
    InputKeyPageUp,
    InputKeyPageDown,
    InputKeyHome,
    InputKeyEnd,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCapsLock,
    InputKeyScrollLock,
    InputKeyNumLock,
    InputKeyPrintScreen,
    InputKeyPauseBreak,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
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
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyNumpad0,
    InputKeyNumpad1,
    InputKeyNumpad2,
    InputKeyNumpad3,
    InputKeyNumpad4,
    InputKeyNumpad5,
    InputKeyNumpad6,
    InputKeyNumpad7,
    InputKeyNumpad8,
    InputKeyNumpad9,
    InputKeyDecimal,
    InputKeyDivide,
    InputKeyMultiply,
    InputKeySubtract,
    InputKeyAdd,
    InputKeyNumpadEnter,
    InputKeyEquals,
    InputKeyCount,
    InputKeyCount,
    InputKeyCount,
    InputKeyLeftShift,
    InputKeyLeftControl,
    InputKeyLeftAlt,
    InputKeyLeftSuper,
    InputKeyRightShift,
    InputKeyRightControl,
    InputKeyRightAlt,
    InputKeyRightSuper,
    InputKeyMenu,
};

VOID InInitialize(VOID)
{
}

VOID InUpdateState(VOID)
{
    // No callback so keys don't change mid-frame
    for (UINT32 i = 0; i < PURPL_ARRAYSIZE(KeyLookupTable); i++)
    {
        if (KeyLookupTable[i] != InputKeyCount)
        {
            INPUT_KEY Mapped = KeyLookupTable[i];
            if (Mapped < InputKeyCount)
            {
                InState.Keyboard[Mapped] = (BOOLEAN)glfwGetKey(VidGetObject(), i); // Only care about up or down
            }
        }
    }
}

VOID InShutdown(VOID)
{
}
