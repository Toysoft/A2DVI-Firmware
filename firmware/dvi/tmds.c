/*
MIT License

Copyright (c) 2024 Thorsten Brehm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "tmds.h"
#include "config/config.h"

// TMDS data for RGB channels for a double pixel (a perfectly bit balanced pixel)
uint32_t DELAYED_COPY_DATA(tmds_mono_double_pixel)[3*4] =
{
    /* R                 G                    B               */
    TMDS_SYMBOL_255_255, TMDS_SYMBOL_255_255, TMDS_SYMBOL_255_255, /* white */
    TMDS_SYMBOL_0_0,     TMDS_SYMBOL_255_255, TMDS_SYMBOL_0_0,     /* green */
    TMDS_SYMBOL_255_255, TMDS_SYMBOL_128_128, TMDS_SYMBOL_0_0,     /* amber */

    TMDS_SYMBOL_0_0,     TMDS_SYMBOL_0_0,     TMDS_SYMBOL_0_0,     /* black */
};

// TMDS data for RGB channels for a pattern of two pixels (a perfectly bit balanced pixel pair)
uint32_t DELAYED_COPY_DATA(tmds_mono_pixel_pair)[4*3*3] =
{
    // white
    /*R*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_255_0, TMDS_SYMBOL_0_255, TMDS_SYMBOL_255_255,
    /*G*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_255_0, TMDS_SYMBOL_0_255, TMDS_SYMBOL_255_255,
    /*B*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_255_0, TMDS_SYMBOL_0_255, TMDS_SYMBOL_255_255,

    // green
    /*R*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_0_0,   TMDS_SYMBOL_0_0,   TMDS_SYMBOL_0_0,
    /*G*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_255_0, TMDS_SYMBOL_0_255, TMDS_SYMBOL_255_255,
    /*B*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_0_0,   TMDS_SYMBOL_0_0,   TMDS_SYMBOL_0_0,

    // amber
    /*R*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_255_0, TMDS_SYMBOL_0_255, TMDS_SYMBOL_255_255,
    /*G*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_128_0, TMDS_SYMBOL_0_128, TMDS_SYMBOL_128_128,
    /*B*/ TMDS_SYMBOL_0_0, TMDS_SYMBOL_0_0,   TMDS_SYMBOL_0_0,   TMDS_SYMBOL_0_0
};

// TMDS symbols for LORES RGB colors - using the "double pixel" trick
// (each symbol covers two pixels and is encoded with a perfect 'bit balance').
uint32_t DELAYED_COPY_DATA(tmds_lorescolor)[3*16] =
{
#if 0
    // R        G        B
    0x7fd00, 0x7fd00, 0x7fd00, // black
    0x8fec0, 0x425f6, 0x906be, // magenta
    0x4f1c3, 0x7911b, 0xbfe00, // darkblue
    0x802ff, 0x4f9c1, 0xbfe00, // purple
    0x7fd00, 0x6017f, 0x906be, // darkgreen
    0x5fd80, 0x5fd80, 0x5fd80, // grey1
    0xae247, 0xb7a21, 0xbfe00, // mediumblue
    0x6f941, 0x882df, 0xbfe00, // lightblue
    0x902bf, 0xa3273, 0x7fd00, // brown
    0xbfa01, 0x9c28f, 0x7fd00, // orange
    0x5fd80, 0x5fd80, 0x5fd80, // grey2
    0xbfe00, 0x5f582, 0x501bf, // pink
    0xaf243, 0x83af1, 0x7fd00, // green
    0x505be, 0xbce0c, 0x7fd00, // yellow
    0x99e98, 0xbfe00, 0x6fd40, // aqua
    0xbfe00, 0xbfe00, 0xbfe00  // white
#else
    // R        G        B
    0x7fd00, 0x7fd00, 0x7fd00,
    0x886de, 0x7e107, 0xa2277,
    0x465e6, 0x465e6, 0x826f6,
    0x85ee8, 0x44dec, 0xbfe00,
    0x7fd00, 0x5fd80, 0x7fd00,
    0x5fd80, 0x5fd80, 0x5fd80,
    0x411fb, 0x5819f, 0xbfe00,
    0xb3233, 0xb3233, 0xbfe00,
    0x73133, 0x73133, 0x7fd00,
    0x812fb, 0x9829f, 0x7fd00,
    0x5fd80, 0x5fd80, 0x5fd80,
    0xbfe00, 0x5e187, 0x826f6,
    0x45de8, 0x84eec, 0x7fd00,
    0x866e6, 0x866e6, 0x425f6,
    0x485de, 0xbe207, 0x62177,
    0xbfe00, 0xbfe00, 0xbfe00
#endif
};

// hires TMDS color patterns
uint32_t DELAYED_COPY_DATA(tmds_hires_color_patterns_red)[2*256] = {
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xded01,
	0xded01,
	0x61fe,
	0x7fd00,
	0xf1d03,
	0xded01,
	0xbfd00,
	0xc0e8f,
	0x9c671,
	0x3fa18,
	0x3fa18,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x8e639,
	0x8e639,
	0x3fa18,
	0x3fa18,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d1c,
	0x8e5e1,
	0x7fd00,
	0xf9d01,
	0x7fd00,
	0xded01,
	0xc0de3,
	0x63d1c,
	0x63d70,
	0x39d70,
	0xc0de3,
	0x71d1c,
	0x63d70,
	0x3f170,
	0xc0d8f,
	0x9cd70,
	0xbfa01,
	0xbfa01,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0x8e639,
	0x8e639,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x7fd00,
	0xded01,
	0x7fd00,
	0xded01,
	0x5c273,
	0x87671,
	0x7fd00,
	0xf1d03,
	0x5c273,
	0x3f28c,
	0xc0e8f,
	0x9c671,
	0x3fa18,
	0x3fa18,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x8ed70,
	0x8ed70,
	0x3fa18,
	0x3fa18,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d1c,
	0x8e5e1,
	0x7fd00,
	0xf9d01,
	0x7fd00,
	0xded01,
	0xc0de3,
	0x63d1c,
	0x63d70,
	0x39d70,
	0xc0de3,
	0x71d1c,
	0x63d70,
	0x3f170,
	0xc0d8f,
	0x9cd70,
	0xbfa01,
	0xbfa01,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xded01,
	0x5c273,
	0x87671,
	0x7fd00,
	0xf1d03,
	0x5c273,
	0x3f28c,
	0xc0e8f,
	0x9c671,
	0x3fa18,
	0x3fa18,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x8e639,
	0x8e639,
	0x3fa18,
	0x3fa18,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d38,
	0x8ed38,
	0xc0de3,
	0x78d1c,
	0xc0de3,
	0x63d1c,
	0xc0de3,
	0x63d1c,
	0x63d70,
	0x39d70,
	0xc0de3,
	0x71d1c,
	0x63d70,
	0x3f170,
	0xc0d8f,
	0x9cd70,
	0xbfa01,
	0xbfa01,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0x8e639,
	0x8e639,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x7fd00,
	0xded01,
	0x7fd00,
	0xded01,
	0x5c273,
	0x87671,
	0x7fd00,
	0xf1d03,
	0x5c273,
	0x3f28c,
	0xc0e8f,
	0x9c671,
	0x3fa18,
	0x3fa18,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x8ed70,
	0x8ed70,
	0x3fa18,
	0x3fa18,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d38,
	0x8ed38,
	0xc0de3,
	0x78d1c,
	0xc0de3,
	0x63d1c,
	0xc0de3,
	0x63d1c,
	0x63d70,
	0x39d70,
	0xc0de3,
	0x71d1c,
	0x63d70,
	0x3f170,
	0x7fe00,
	0x232fc,
	0x806fe,
	0x806fe,
	0xf1e03,
	0xf1e03,
	0x62fe,
	0x62fe,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x7fd00,
	0xded01,
	0xded01,
	0xbfd00,
	0x61fe,
	0xbfd00,
	0xbfd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x7fd00,
	0x7fd00,
	0x5c273,
	0x5c273,
	0x87639,
	0x87639,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x8e639,
	0x9c639,
	0x8e639,
	0x9c639,
	0x63d70,
	0x63d70,
	0x63d70,
	0x63d70,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x63d70,
	0x63d70,
	0x63d70,
	0x63d70,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x9c671,
	0x4e273,
	0x8ed70,
	0x9cd70,
	0xbfa01,
	0xb9e03,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xb9e03,
	0xbfa01,
	0x3fe01,
	0xc0dc7,
	0x71d38,
	0x9c671,
	0x9c671,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x8761d,
	0x8761d,
	0xbfa01,
	0xbfa01,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x8e639,
	0x9c639,
	0x8e639,
	0x9c639,
	0x8e639,
	0x9c639,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0xc0e8f,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x78d1c,
	0xc0de3,
	0x5c273,
	0x5c273,
	0x3fa18,
	0x8761d,
	0x3fa18,
	0x3fa18,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x7fd00,
	0x7fd00,
	0x5c273,
	0x5c273,
	0x87639,
	0x87639,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x8e639,
	0x9c639,
	0x8e639,
	0x9c639,
	0x63d70,
	0x63d70,
	0x63d70,
	0x63d70,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x63d70,
	0x63d70,
	0x63d70,
	0x63d70,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x9c671,
	0x4e273,
	0x8ed70,
	0x9cd70,
	0xbfa01,
	0xb9e03,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xb9e03,
	0xbfa01,
	0x3fe01,
	0xc0dc7,
	0x71d38,
	0x9c671,
	0x9c671,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x8761d,
	0x8761d,
	0xbfa01,
	0xbfa01,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x8e639,
	0x9c639,
	0x8e639,
	0x9c639,
	0x8e639,
	0x9c639,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0x7fe00,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00
};

uint32_t DELAYED_COPY_DATA(tmds_hires_color_patterns_green)[2*256] = {
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0xded01,
	0x231fc,
	0xded01,
	0x231fc,
	0xbfd00,
	0xbfd00,
	0x7fd00,
	0x7fd00,
	0x71d1c,
	0x71d1c,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x78d1c,
	0x78d1c,
	0x9cd38,
	0x9cd38,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d1c,
	0x71d1c,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0xc0de3,
	0x78d1c,
	0x63d70,
	0x9cd70,
	0x5c23b,
	0x9c639,
	0xbfa01,
	0x3fe01,
	0xc0de3,
	0xc0de3,
	0x71d38,
	0x71d38,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0x78d38,
	0x78d38,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0xc0de3,
	0x78d1c,
	0x63d70,
	0x9cd70,
	0x63d70,
	0x9cd70,
	0x3fa18,
	0x3fa18,
	0xc0de3,
	0xc0de3,
	0x71d38,
	0x71d38,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x78d38,
	0x78d38,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d38,
	0x71d38,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0x78d38,
	0x78d38,
	0x5c23b,
	0x9c639,
	0x5c23b,
	0x9c639,
	0xbfa01,
	0x3fe01,
	0xc0dc7,
	0xc0dc7,
	0x4e273,
	0x4e273,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x5c273,
	0x9c671,
	0x63d70,
	0x9cd70,
	0x3fa18,
	0x3fa18,
	0x7fd00,
	0x7fd00,
	0x71d38,
	0x71d38,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x78d1c,
	0x78d1c,
	0x9cd38,
	0x9cd38,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d38,
	0x71d38,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0xc0de3,
	0x78d1c,
	0x63d70,
	0x9cd70,
	0x5c23b,
	0x9c639,
	0xbfa01,
	0x3fe01,
	0xc0de3,
	0xc0de3,
	0x71d38,
	0x71d38,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0x78d38,
	0x78d38,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0xc0de3,
	0x78d1c,
	0x63d70,
	0x9cd70,
	0x63d70,
	0x9cd70,
	0x3fa18,
	0x3fa18,
	0xc0de3,
	0xc0de3,
	0x71d38,
	0x71d38,
	0x71d38,
	0x71d38,
	0x87639,
	0x87639,
	0x78d38,
	0x78d38,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78671,
	0x4e273,
	0x4e273,
	0x4e273,
	0x8761d,
	0x8761d,
	0x78d38,
	0x78d38,
	0x5c23b,
	0x9c639,
	0x5c23b,
	0x9c639,
	0xbfa01,
	0x3fe01,
	0x7fe00,
	0x7fe00,
	0xf1e03,
	0xf1e03,
	0xf1e03,
	0xf1e03,
	0x62fe,
	0x62fe,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0xdbd01,
	0xded01,
	0x61fe,
	0xf1d03,
	0xdbd01,
	0x231fc,
	0xbfd00,
	0xc0dc7,
	0x71d38,
	0x5c273,
	0x5c273,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x8761d,
	0x8761d,
	0x3f170,
	0x3f170,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x231fc,
	0x78d1c,
	0x71d1c,
	0x78d1c,
	0x71d1c,
	0x63d70,
	0x39d70,
	0x63d70,
	0x39d70,
	0x9c671,
	0x87671,
	0x9c671,
	0x3f28c,
	0xc0d8f,
	0x63d70,
	0x63d70,
	0x63d70,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x78d1c,
	0x71d1c,
	0x71d38,
	0x8ed38,
	0x9c671,
	0x87671,
	0x71d38,
	0x8ed38,
	0x9c671,
	0x3f28c,
	0xc0dc7,
	0x71d38,
	0x9c671,
	0x9c671,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x8761d,
	0x8761d,
	0xbfa01,
	0xbfa01,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d1c,
	0x71d1c,
	0x78d1c,
	0x71d1c,
	0x78d1c,
	0x71d1c,
	0x9c671,
	0x87671,
	0x9c671,
	0x87671,
	0x9c671,
	0x87671,
	0x9c671,
	0x3f28c,
	0xc0e8f,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x78d1c,
	0x8e5e1,
	0x5c273,
	0x87671,
	0x71d38,
	0x8ed38,
	0x9c671,
	0x3f28c,
	0xc0dc7,
	0x71d38,
	0x5c273,
	0x5c273,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x8761d,
	0x8761d,
	0x3f170,
	0x3f170,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x231fc,
	0x78d1c,
	0x71d1c,
	0x78d1c,
	0x71d1c,
	0x63d70,
	0x39d70,
	0x63d70,
	0x39d70,
	0x9c671,
	0x87671,
	0x9c671,
	0x3f28c,
	0xc0d8f,
	0x63d70,
	0x63d70,
	0x63d70,
	0x9c671,
	0x9c671,
	0x9c671,
	0x9c671,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x78d1c,
	0x71d1c,
	0x71d38,
	0x8ed38,
	0x9c671,
	0x87671,
	0x71d38,
	0x8ed38,
	0x9c671,
	0x3f28c,
	0xc0dc7,
	0x71d38,
	0x9c671,
	0x9c671,
	0x71d38,
	0x71d38,
	0x9c671,
	0x9c671,
	0x8761d,
	0x8761d,
	0xbfa01,
	0xbfa01,
	0x8761d,
	0x8761d,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x78d1c,
	0x71d1c,
	0x78d1c,
	0x71d1c,
	0x78d1c,
	0x71d1c,
	0x9c671,
	0x87671,
	0x9c671,
	0x87671,
	0x9c671,
	0x87671,
	0x9c671,
	0x3f28c,
	0x7fe00,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0x232fc,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00
};

uint32_t DELAYED_COPY_DATA(tmds_hires_color_patterns_blue)[2*256] = {
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xdbd01,
	0x61fe,
	0xded01,
	0x61fe,
	0xbfd00,
	0xbfd00,
	0xdbd01,
	0xbfd00,
	0xc0e8f,
	0x9c671,
	0xc0de3,
	0xc0de3,
	0x71d38,
	0x71d38,
	0xc0de3,
	0xc0de3,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf1d03,
	0x78d1c,
	0xc0de3,
	0x8e639,
	0x87639,
	0x63d70,
	0x39d70,
	0x3fa18,
	0x3fa18,
	0x8e639,
	0x3f2c4,
	0xc0dc7,
	0x9cd38,
	0x7fd00,
	0x7fd00,
	0x71d1c,
	0x71d1c,
	0x7fd00,
	0x7fd00,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x78d1c,
	0xc0de3,
	0x8e639,
	0x3f2c4,
	0x5c23b,
	0x87639,
	0xbfa01,
	0xbfa01,
	0x8e639,
	0x3f2c4,
	0xc0e8f,
	0x9c671,
	0xc0de3,
	0xc0de3,
	0x71d38,
	0x71d38,
	0xc0de3,
	0xc0de3,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf1d03,
	0x78d1c,
	0xc0de3,
	0x8e639,
	0x87639,
	0x63d70,
	0x39d70,
	0x3fa18,
	0x3fa18,
	0x8e639,
	0x3f2c4,
	0xc0dc7,
	0x9cd38,
	0x7fd00,
	0x7fd00,
	0x71d1c,
	0x71d1c,
	0x7fd00,
	0x7fd00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x8e639,
	0x87639,
	0x63d70,
	0x39d70,
	0xbfa01,
	0xbfa01,
	0x8e639,
	0x3f2c4,
	0xc0d8f,
	0x9cd70,
	0xc0dc7,
	0xc0dc7,
	0x4e273,
	0x4e273,
	0xc0dc7,
	0xc0dc7,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0xc0de3,
	0xc0de3,
	0xc0de3,
	0x71d1c,
	0x78d38,
	0xc0dc7,
	0x8e639,
	0x87639,
	0x63d70,
	0x39d70,
	0x3fa18,
	0x3fa18,
	0x8e639,
	0x3f2c4,
	0xc0d8f,
	0x9cd70,
	0xc0dc7,
	0xc0dc7,
	0x4e273,
	0x4e273,
	0xc0dc7,
	0xc0dc7,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xc0dc7,
	0x78d38,
	0x78671,
	0xc0e8f,
	0x8e639,
	0x3f2c4,
	0x5c23b,
	0x87639,
	0xbfa01,
	0xbfa01,
	0x8e639,
	0x3f2c4,
	0xc0d8f,
	0x9cd70,
	0xc0dc7,
	0xc0dc7,
	0x4e273,
	0x4e273,
	0xc0dc7,
	0xc0dc7,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0xc0de3,
	0xc0de3,
	0xc0de3,
	0x71d1c,
	0x78d38,
	0xc0dc7,
	0x8e639,
	0x87639,
	0x63d70,
	0x39d70,
	0x3fa18,
	0x3fa18,
	0x8e639,
	0x3f2c4,
	0x7fe00,
	0x232fc,
	0x7fe00,
	0x7fe00,
	0xf1e03,
	0xf1e03,
	0x7fe00,
	0x7fe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0xded01,
	0xded01,
	0x7fd00,
	0x7fd00,
	0x61fe,
	0xbfd00,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x87639,
	0x71d38,
	0x71d38,
	0x3f2c4,
	0x3f2c4,
	0x78d1c,
	0x78d1c,
	0xdbe01,
	0xdbe01,
	0x7fd00,
	0x7fd00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x8e639,
	0x87639,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x63d70,
	0x8ed70,
	0x63d70,
	0x8ed70,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0x7fa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x8e639,
	0x8e639,
	0x8e639,
	0x8e639,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x9c671,
	0x4e273,
	0x3fa18,
	0x3fa18,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x87639,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x3f2c4,
	0xc0dc7,
	0x71d38,
	0x3fa18,
	0x3fa18,
	0x71d38,
	0x71d38,
	0x3fa18,
	0x3fa18,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x87639,
	0x7fd00,
	0x7fd00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0x7fa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0xf9d01,
	0x5c273,
	0x5c273,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x3f2c4,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x87639,
	0x71d38,
	0x71d38,
	0x3f2c4,
	0x3f2c4,
	0x78d1c,
	0x78d1c,
	0xdbe01,
	0xdbe01,
	0x7fd00,
	0x7fd00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x8e639,
	0x87639,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x63d70,
	0x8ed70,
	0x63d70,
	0x8ed70,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0x7fa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x8e639,
	0x8e639,
	0x8e639,
	0x8e639,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x7fd00,
	0x9c671,
	0x4e273,
	0x3fa18,
	0x3fa18,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x87639,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x3f2c4,
	0xc0dc7,
	0x71d38,
	0x3fa18,
	0x3fa18,
	0x71d38,
	0x71d38,
	0x3fa18,
	0x3fa18,
	0x7fd00,
	0x7fd00,
	0x87639,
	0x87639,
	0x7fd00,
	0x7fd00,
	0xbfe00,
	0xbfe00,
	0x7fd00,
	0x7fd00,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0xbfa01,
	0x3fe01,
	0x7fe00,
	0x806fe,
	0x806fe,
	0x806fe,
	0x806fe,
	0x806fe,
	0x806fe,
	0x806fe,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00,
	0xbfe00
};
