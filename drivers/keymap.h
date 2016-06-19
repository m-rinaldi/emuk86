#pragma once

#define LCTRL   0x1d
#define LSHIFT  0x2a
#define RSHIFT  0x36

// lookup table: scancode --> ASCII code
static const char _keymap[][2] = {
    {  0 ,  0  },       // 0x00     
    {  0 ,  0  },       // 0x01     
    { '1', '!' },       // 0x02     
    { '2',  0  },       // 0x03     
    { '3',  0  },       // 0x04     
    { '4', '$' },       // 0x05     
    { '5', '%' },       // 0x06     
    { '6',  0  },       // 0x07     
    { '7',  0  },       // 0x08     
    { '8',  0  },       // 0x09     
    { '9',  0  },       // 0x0a     
    { '0',  0  },       // 0x0b     
    {  0 ,  0  },       // 0x0c     
    {  0 ,  0  },       // 0x0d     
    { 127,  0  },       // 0x0e     backspace (DEL) 
    {  0 ,  0  },       // 0x0f     
    { 'q', 'Q' },       // 0x10     
    { 'w', 'W' },       // 0x11     
    { 'e', 'E' },       // 0x12     
    { 'r', 'R' },       // 0x13     
    { 't', 'T' },       // 0x14     
    { 'y', 'Y' },       // 0x15     
    { 'u', 'U' },       // 0x16     
    { 'i', 'I' },       // 0x17     
    { 'o', 'O' },       // 0x18     
    { 'p', 'P' },       // 0x19     
    { '`',  0  },       // 0x1a     
    { '+',  0  },       // 0x1b     
    {'\n',  0  },       // 0x1c     enter 
    { 'X',  0  },       // 0x1d     
    { 'a', 'A' },       // 0x1e     
    { 's', 'S' },       // 0x1f     
    { 'd', 'D' },       // 0x20     
    { 'f', 'F' },       // 0x21     
    { 'g', 'G' },       // 0x22     
    { 'h', 'H' },       // 0x23     
    { 'j', 'J' },       // 0x24     
    { 'k', 'K' },       // 0x25     
    { 'l', 'L' },       // 0x26     
    { 'Y',  0  },       // 0x27     
    { 'Y',  0  },       // 0x28     
    { 'Y',  0  },       // 0x29     
    { 'X',  0  },       // 0x2a     rshift 
    { 'Y',  0  },       // 0x2b     
    { 'z', 'Z' },       // 0x2c     
    { 'x', 'X' },       // 0x2d     
    { 'c', 'C' },       // 0x2e     
    { 'v', 'V' },       // 0x2f     
    { 'b', 'B' },       // 0x30     
    { 'n', 'N' },       // 0x31     
    { 'm', 'M' },       // 0x32     
    { ',',  0  },       // 0x33     
    { '.',  0  },       // 0x34     
    { '-',  0  },       // 0x35     
    { 'X',  0  },       // 0x36     lshift 
    { 'Z',  0  },       // 0x37     
    { 'Z',  0  },       // 0x38     LAlt 
    { ' ', ' ' },       // 0x39     space bar 
    { 'Z',  0  },       // 0x3a     CapsLock 
    {  0 ,  0  },       // 0x3b     F1 
    {  0 ,  0  },       // 0x3c     F2 
    {  0 ,  0  },       // 0x3d     F3 
    {  0 ,  0  },       // 0x3e     F4 
    {  0 ,  0  },       // 0x3f     F5 
    {  0 ,  0  },       // 0x40     F6 
    {  0 ,  0  },       // 0x41     F7 
    {  0 ,  0  },       // 0x42     F8 
    {  0 ,  0  },       // 0x43     F9 
    {  0 ,  0  },       // 0x44     F10 
    {  0 ,  0  },       // 0x45     NumLock 
    {  0 ,  0  },       // 0x46     ScrollLock 
    {  0 ,  0  },       // 0x47     Keypad-7 
    {  0 ,  0  },       // 0x48     Keypad-8/Up 
    {  0 ,  0  },       // 0x49     Keypad-9/PgUp 
    {  0 ,  0  },       // 0x4a     Keypad-- 
    {  0 ,  0  },       // 0x4b     Keypad-4/Left 
    {  0 ,  0  },       // 0x4c     Keypad-5 
    {  0 ,  0  },       // 0x4d     Keypad-6/Right 
    {  0 ,  0  },       // 0x4e     Keypad--+ 
    {  0 ,  0  },       // 0x4f     Keypad-1/End 
    {  0 ,  0  },       // 0x50     Keypad-2/Down 
    {  0 ,  0  },       // 0x51     Keypad-3/PgDn 
    {  0 ,  0  },       // 0x52     Keypad-0/Ins 
    {  0 ,  0  },       // 0x53     Keypad-/Del 
    {  0 ,  0  },       // 0x54     Alt-SysRq 
    {  0 ,  0  },       // 0x55     
    {  0 ,  0  },       // 0x56     
    {  0 ,  0  },       // 0x57     F11 
    {  0 ,  0  },       // 0x58     F12 
    {  0 ,  0  },       // 0x59     
    {  0 ,  0  },       // 0x5a     
    {  0 ,  0  },       // 0x5b     
    {  0 ,  0  },       // 0x5c     
    {  0 ,  0  },       // 0x5d     
    {  0 ,  0  },       // 0x5e     
    {  0 ,  0  },       // 0x5f     
    {  0 ,  0  },       // 0x60     
    {  0 ,  0  },       // 0x61     
    {  0 ,  0  },       // 0x62     
    {  0 ,  0  },       // 0x63     
    {  0 ,  0  },       // 0x64     
    {  0 ,  0  },       // 0x65     
    {  0 ,  0  },       // 0x66     
    {  0 ,  0  },       // 0x67     
    {  0 ,  0  },       // 0x68     
    {  0 ,  0  },       // 0x69     
    {  0 ,  0  },       // 0x6a     
    {  0 ,  0  },       // 0x6b     
    {  0 ,  0  },       // 0x6c     
    {  0 ,  0  },       // 0x6d     
    {  0 ,  0  },       // 0x6e     
    {  0 ,  0  },       // 0x6f     
    {  0 ,  0  },       // 0x70     
    {  0 ,  0  },       // 0x71     
    {  0 ,  0  },       // 0x72     
    {  0 ,  0  },       // 0x73     
    {  0 ,  0  },       // 0x74     
    {  0 ,  0  },       // 0x75     
    {  0 ,  0  },       // 0x76     
    {  0 ,  0  },       // 0x77     
    {  0 ,  0  },       // 0x78     
    {  0 ,  0  },       // 0x79     
    {  0 ,  0  },       // 0x7a     
    {  0 ,  0  },       // 0x7b     
    {  0 ,  0  },       // 0x7c     
    {  0 ,  0  },       // 0x7d     
    {  0 ,  0  },       // 0x7e     
    {  0 ,  0  },       // 0x7f     
};