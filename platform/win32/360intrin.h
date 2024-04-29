#pragma once

#ifdef PURPL_XBOX360
#define BitScanReverse32(result, value) \
    __asm { \
        cntlzw result, value \
        subi result, result, 32 \
        li 3, 31 \
1:      \
        beq 2f \
        slwi 0, 1, 5 \
        and. 0, 0, value \
        beq 2f \
        mr result, 3 \
2:      \
        bdnz 1b \
    }

#define BitScanForward32(result, value) \
    __asm { \
        cntlzw result, value \
        subi result, result, 32 \
        li 3, 0 \
1:      \
        beq 2f \
        slwi 0, 1, 5 \
        and. 0, 0, value \
        beq 2f \
        mr result, 3 \
2:      \
        bdz 1b \
    }

#define _mm_prefetch(address, hint) \
    __asm { \
        nop \
    }
#endif
