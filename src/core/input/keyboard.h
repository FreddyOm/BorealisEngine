#pragma once
#include "../../config.h"
#include <bitset>
#include "../helpers/events.h"
#include "../types/types.h"


namespace Borealis::Input
{
	enum class KEYBOARD_KEYS : Types::uint16
	{
        LMB = 0x01,
        RMB = 0x02,
		CONTROL_BREAK = 0x03,
        MMB = 0x04,
        X1MB = 0x05,
        X2MB = 0x06,
        // 0x07 RESERVED
        BACKSPACE = 0x08,
        TAB = 0x09,
        // 0x0A - 0x0B RESERVED
        CLEAR = 0x0C,
        ENTER = 0x0D,
		// 0x0E-0F UNASSIGNED
        SHIFT = 0x10,
        CTRL = 0x11,
        ALT = 0x12,
        PAUSE = 0x13,
        CAPS_LOCK = 0x14,
        IME_KANA_HANGUL_MODE = 0x15,
        IME_ON = 0x16,
        IME_JUNJA = 0x17,
        IME_FINAL = 0x18,
        IME_HANJA_KANJI = 0x19,
        IME_OFF = 0x1A,
        ESC = 0x1B,
        IME_CONVERT = 0x1C,
        IME_NONCONVERT = 0x1D,
        IME_ACCEPT = 0x1E,
        IME_MODE_CHANGE_REQ = 0x1F,
        SPACEBAR = 0x20,
        PAGE_UP = 0x21,
        PAGE_DOWN = 0x22,
        END = 0x23,
        HOME = 0x24,
        LEFT = 0x25,
        UP = 0x26,
        RIGHT = 0x27,
        DOWN = 0x28,
        SELECT = 0x29,
        PRINT = 0x2A,
        EXECUTE = 0x2B,
        PRTSC = 0x2C,
        INS = 0x2D,
        DEL = 0x2E,
        HELP = 0x2F,
        _0 = 0x30,
        _1 = 0x31,
        _2 = 0x32,
        _3 = 0x33,
        _4 = 0x34,
        _5 = 0x35,
        _6 = 0x36,
        _7 = 0x37,
        _8 = 0x38,
        _9 = 0x39,
		// 0x3A-40 UNDEFINED
        A = 0x41,
        B = 0x42,
        C = 0x43,
        D = 0x44,
        E = 0x45,
        F = 0x46,
        G = 0x47,
        H = 0x48,
        I = 0x49,
        J = 0x4A,
        K = 0x4B,
        L = 0x4C,
        M = 0x41D,
        N = 0x4E,
        O = 0x4F,
        P = 0x50,
        Q = 0x51,
        R = 0x52,
        S = 0x53,
        T = 0x54,
        U = 0x55,
        V = 0x56,
        W = 0x57,
        X = 0x58,
        Y = 0x59,
        Z = 0x5A,
        LWIN = 0x5B,
        RWIN = 0x5C,
        APP = 0x5D,
        // 0x5E RESERVED
        SLEEP = 0x5F,
        NUM_0 = 0x60,
        NUM_1 = 0x61,
        NUM_2 = 0x62,
        NUM_3 = 0x63,
        NUM_4 = 0x64,
        NUM_5 = 0x65,
        NUM_6 = 0x66,
        NUM_7 = 0x67,
        NUM_8 = 0x68,
        NUM_9 = 0x69,
        MULTIPLY = 0x6A,
        ADD = 0x6B,
        SEPARATO = 0x6C,
        SUBSTRACT = 0x6D,
        DECIMAL = 0x6E,
        DIVIDE = 0x6F,
        F1 = 0x70,
        F2 = 0x71,
        F3 = 0x72,
        F4 = 0x73,
        F5 = 0x74,
        F6 = 0x75,
        F7 = 0x76,
        F8 = 0x77,
        F9 = 0x78,
        F10 = 0x79,
        F11 = 0x7A,
        F12 = 0x7B,
        F13 = 0x7C,
        F14 = 0x7D,
        F15 = 0x7E,
        F16 = 0x7F,
        F17 = 0x80,
        F18 = 0x81,
        F19 = 0x82,
        F20 = 0x83,
        F21 = 0x84,
        F22 = 0x85,
        F23 = 0x86,
        F24 = 0x87,
        // 0x88-0x8F RESERVED,
        NUM_LOCK = 0x90,
        SCROLL_LOCK = 0x91,
        // 0x92 - 0x96 OEM SPECIFIC
		// 0x97 - 0x9F UNASSIGNED
        LSHIFT = 0xA0,
        RSHIFT = 0xA1,
        LCTRL = 0xA2,
        RCTRL = 0xA3,
        LALT = 0xA4,
        RALT = 0xA5,
        BR_BACK = 0xA6,
        BR_FORW = 0xA7,
        BR_REF = 0xA8,
        BR_STOP = 0xA9,
        BR_SEARCH = 0xAA,
        BR_FAV = 0xAB,
        BR_START = 0xAC,
        VOL_MUTE = 0xAD,
        VOL_DOWN = 0xAE,
        VOL_UP = 0xAF,
        NEXT_TRACK = 0xB0,
        PREV_TRACK = 0xB1,
        STOP_MEDIA = 0xB2,
        PLAY_PAUSE_MEDIA = 0xB3,
        START_MAIL = 0xB4,
        SELECT_MEDIA = 0xB5,
        START_APP1 = 0xB6,
        START_APP2 = 0xB7,
        // 0xB8 - 0xB9 RESERVED
        SEMI_COLON = 0xBA,
        PLUS = 0xBB,
        COMMA = 0xBC,
        DASH = 0xBD,
        PERIOD = 0xBE,
        FORWARD_SLASH = 0xBF,
        GRAVE_ACCENT = 0xC0,
        // 0xC1 - 0xDA RESERVED
        LEFT_BRACE = 0xDB,
        BACK_SLASH = 0xDC,
        RIGHT_BRACE = 0xDD,
        APOSTROPHE = 0xDE,
        MISC = 0xDF,
		// 0xE0 RESERVED
        // 0xE1 OEM SPECIFIC
        PIPE = 0xE2,
        // 0xE3 - 0xE4 OEM SPECIFIC
        IME_PROCESS = 0xE5,
        // 0xE6 OEM SPECIFIC
        PACKET = 0xE7,
		// 0xE8 UNASSIGNED
		// 0xE9 - 0xF5 OEM SPECIFIC
        ATTN = 0xF6,
        CRSEL = 0xF7,
        EXSEL = 0xF8,
        ERASE_EOF = 0xF9,
        PLAY = 0xFA,
        ZOOM = 0xFB,
        // 0xFC RESERVED
        PA1 = 0xFD,
        OEM_CLEAR = 0xFE,
        KEYBOARD_KEYS_SIZE = 0xFF
    };

	//class Keyboard
	//{
	//	public:
	//	Keyboard() = default;
	//	~Keyboard() = default;

	//	void DetectButtonUpDownEvents();
 //       Borealis::Events::Action<KEYBOARD_KEYS> onKeyDown;
 //       Borealis::Events::Action<KEYBOARD_KEYS> onKeyUp;

	//private:
	//	std::bitset<256> prevKeyboardKeyStates{ 0 };
	//	std::bitset<256> keyboardKeyStates{ 0 };
	//};
}