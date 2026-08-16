#pragma once

enum class HIT_LEVEL { HIT_NORMAL, HIT_STRONG, HIT_END };
enum class HIT_DIR { HIT_FL, HIT_FR, HIT_BL, HIT_BR, END };
enum HIT_KEY {
    // Level (상위 비트)
    HIT_NORMAL = 0x0000,
    HIT_STRONG = 0x0100,

    // Direction (하위 비트)
    DIR_FL = 0x0001,
    DIR_FR = 0x0002,
    DIR_BL = 0x0003,
    DIR_BR = 0x0004,
};


inline _int MakeHitKey(HIT_LEVEL eLevel, HIT_DIR eDir) {
    return (_int)eLevel << 8 | (_int)eDir;
}

