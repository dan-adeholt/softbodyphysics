#ifndef __GAME__GAME_KEY_CODE_H
#define __GAME__GAME_KEY_CODE_H

enum class GameModkey : int
{
    None = 0,
    Shift = 1,
    Ctrl = 1 << 1,
    Alt = 1 << 2,
    NUM_MODKEYS
};

enum class GameKeyCode : int
{
    ESCAPE = 0,
    BACKSPACE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    PLUS,
    MINUS,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    NUM_KEY_CODES
};

#endif