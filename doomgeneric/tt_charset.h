#ifndef TT_CHARSET_H
#define TT_CHARSET_H

// from https://galax.xyz/TELETEXT/CHARSET.HTM
//    
// 0x80 Nothing          0x90 Nothing
// 0x81 Alpha Red        0x91 Graphic Red
// 0x82 Alpha Green      0x92 Graphic Green
// 0x83 Alpha Yellow     0x93 Graphic Yellow
// 0x84 Alpha Blue       0x94 Graphic Blue
// 0x85 Alpha Magenta    0x95 Graphic Magenta
// 0x86 Alpha Cyan       0x96 Graphic Cyan
// 0x87 Alpha White *    0x97 Graphic White
// 0x88 Flash            0x98 Conceal Display
// 0x89 Steady *         0x99 Contiguous Graphics *
// 0x8A Nothing          0x9A Separated Graphics
// 0x8B Nothing          0x9B Nothing
// 0x8C Normal Height *  0x9C Black Background *
// 0x8D Double height    0x9D New Background
// 0x8E Nothing          0x9E Hold Graphics
// 0x8F Nothing          0x9F Release Graphics *
// * every line starts with these options

#define TTEXT_ALPHA_RED 0x81
#define TTEXT_ALPHA_GREEN 0x82
#define TTEXT_ALPHA_YELLOW 0x83
#define TTEXT_ALPHA_BLUE 0x84
#define TTEXT_ALPHA_MAGENTA 0x85
#define TTEXT_ALPHA_CYAN 0x86
#define TTEXT_ALPHA_WHITE 0x87
#define TTEXT_FLASH 0x88
#define TTEXT_STEADY 0x89
#define TTEXT_NORMAL_HEIGHT 0x8C
#define TTEXT_DOUBLE_HEIGHT 0x8D
#define TTEXT_GRAPHIC_RED 0x91
#define TTEXT_GRAPHIC_GREEN 0x92
#define TTEXT_GRAPHIC_YELLOW 0x93
#define TTEXT_GRAPHIC_BLUE 0x94
#define TTEXT_GRAPHIC_MAGENTA 0x95
#define TTEXT_GRAPHIC_CYAN 0x96
#define TTEXT_GRAPHIC_WHITE 0x97
#define TTEXT_CONCEAL_DISPLAY 0x98
#define TTEXT_CONTIGUOUS_GRAPHICS 0x99
#define TTEXT_SEPARATED_GRAPHICS 0x9A
#define TTEXT_BLACK_BACKGROUND 0x9C
#define TTEXT_NEW_BACKGROUND 0x9D
#define TTEXT_HOLD_GRAPHICS 0x9E
#define TTEXT_RELEASE_GRAPHICS 0x9F

#endif