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

/*
 * This file contains test cases. These are only built when the TEST
 * firmware is compiled. The test firmware disables the normal 6502
 * bus interface and simulates some write operations, cycling through
 * all supported video modes and settings.
 */

#include "pico/time.h"
#include "debug/debug.h"
#include "menu/menu.h"
#include "applebus/abus.h"
#include "applebus/buffers.h"
#include "applebus/businterface.h"
#include "applebus/abus_pin_config.h"
#include "config/config.h"
#include "duck.h"

#ifdef FEATURE_TEST

#define REG_SW_40COL      0xc00c
#define REG_SW_80COL      0xc00d
#define REG_SW_NORMCHAR   0xc00e
#define REG_SW_ALTCHAR    0xc00f

#define REG_SW_MONOCHROME 0xc021

#define REG_SW_TEXT_OFF   0xc050  // enables LORES by default
#define REG_SW_TEXT       0xc051
#define REG_SW_MIX_OFF    0xc052
#define REG_SW_MIX        0xc053
#define REG_SW_PAGE_1     0xc054
#define REG_SW_PAGE_2     0xc055
#define REG_SW_HIRES_OFF  0xc056
#define REG_SW_HIRES      0xc057

#define REG_SW_DGR        0xc05e
#define REG_SW_DGR_OFF    0xc05f

#define REG_CARD          (0xC080 | 0x30)

#define TEST_40_COLUMNS
#define TEST_80_COLUMNS
#define TEST_LORES
#define TEST_HIRES
#define TEST_DOUBLE_LORES

#define TEST_MIX_MODES

#define TEST_PAGE_SWITCH
#define TEST_ALTCHAR_SWTICH

//#define TEST_MENU

const uint32_t TestDelaySeconds = 5;
const uint32_t TestDelayMilliseconds = TestDelaySeconds*1000;

void clearBothPages()
{
    PrintModePage2 = true;
    clearTextScreen();
    PrintModePage2 = false;
    clearTextScreen();
}

// simulate a write access with given address/data
static void simulateWrite(uint16_t address, uint8_t data)
{
    uint32_t card_select = (1u << (CONFIG_PIN_APPLEBUS_DEVSEL - CONFIG_PIN_APPLEBUS_DATA_BASE));
    if ((address & 0xCFF0) == (0xC080+0x30)) // Slot 3 register area
        card_select = 0;
    businterface((address << 11) | data | card_select);
}

void sleep(int Milliseconds)
{
    while (Milliseconds--)
    {
#if 1
        if (0 == Milliseconds % 100)
            gpio_xor_mask(1u << PICO_DEFAULT_LED_PIN);
#endif
        debug_check_bootsel();
        sleep_ms(1);
    }
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
}

void togglePages()
{
#ifdef TEST_PAGE_SWITCH
    for (uint i=0;i<2;i++)
    {
        simulateWrite(REG_SW_PAGE_2, 0); // show page 2
        sleep(500);
        simulateWrite(REG_SW_PAGE_1, 0); // show page 1
        sleep(500);
    }
#endif
}

void toggleAltChar()
{
#ifdef TEST_ALTCHAR_SWTICH
    // toggle mousetext vs default character set
    for (uint i=0;i<3;i++)
    {
        simulateWrite(REG_SW_ALTCHAR, 0);
        sleep(500);
        simulateWrite(REG_SW_NORMCHAR, 0);
        sleep(500);
    }
#endif
}

void testPrintChar(uint32_t x, uint32_t line, char c)
{
    uint32_t ScreenOffset = (((line & 0x7) << 7) + (((line >> 3) & 0x3) * 40));
    char* pScreenArea = ((char*) text_p1) + ScreenOffset;
    if (PrintModePage2)
        pScreenArea = ((char*) text_p2) + ScreenOffset;
    char* pScreenArea80 = ((PrintModePage2) ? ((char*)text_p4) + ScreenOffset : ((char*)text_p3) + ScreenOffset);

    if (PrintMode80Column)
    {
        if (x&1)
            pScreenArea[x>>1] = c;
        else
            pScreenArea80[x>>1] = c;
    }
    else
    {
        pScreenArea[x] = c;
    }
}

void setLoresPixel(uint16_t x, uint8_t y, bool page2, uint8_t color)
{
     if (page2)
        x = x | 1024;

     color &= 0xF;
     uint8_t mask = 0xF0; // even rows in low nibble

     if ((y & 1) == 1)
     {
         // odd rows in high nibble
         mask = 0x0F;
         color <<= 4;
     }

     uint line = y >> 1;
     uint16_t address = 0x400+((line & 0x7) << 7) + (((line >> 3) & 0x3) * 40)+x;
     apple_memory[address] = (apple_memory[address] & mask) | color;
}

void setLoresTestPattern(uint lines)
{
    for (uint y=0;y<lines;y++)
    {
        for (uint x=0;x<40;x++)
        {
            setLoresPixel(   x, y, false, (x+y)&0xf);  // page 1
            setLoresPixel(39-x, y, true,  (x+y)&0xf);  // page 2
        }
    }
}

void setHiresTestPattern()
{
    for (uint i=0;i<sizeof(A_DUCK_BIN)/4;i++)
    {
        uint32_t data = ((uint32_t*)A_DUCK_BIN)[i];
        ((uint32_t*)hgr_p1)[i] = data;
        ((uint32_t*)hgr_p2)[i] = data ^ 0xffffffff;
    }
}

void test40columns()
{
#ifdef TEST_40_COLUMNS
    clearBothPages();

    // prepare PAGE2
    PrintModePage2 = true;
    printXY(20-3,11, "PAGE 2", PRINTMODE_NORMAL);

    // prepare PAGE1
    PrintModePage2 = false;
    for (uint x=0;x<40;x++)
    {
        char s[3];
        if ((x&0xf)<10)
            s[0] = '0'+(x&0xf);
        else
            s[0] = 'A'+(x&0xf)-10;
        s[1] = 0;
        printXY(x,0, s, PRINTMODE_NORMAL);
        if (x<24)
            printXY(0,x, s, PRINTMODE_NORMAL);
    }
    printXY(20-13,2, "A2DVI Test: 40 column mode", PRINTMODE_NORMAL);
    for (uint i=0;i<255;i++)
    {
        testPrintChar((20-8)+(i&0xf), 4+(i>>4), i);
    }

    simulateWrite(REG_SW_40COL, 0);          // disable 80column mode
    sleep(TestDelayMilliseconds);

    togglePages();                           // test both pages
    toggleAltChar();                         // test mouse text
#endif
}

void test80columns()
{
#ifdef TEST_80_COLUMNS
    PrintMode80Column = true;
    simulateWrite(REG_SW_80COL, 0); // enable 80column mode

    clearBothPages();

    // prepare PAGE2
    PrintModePage2 = true;
    printXY(40-3,11, "PAGE 2", PRINTMODE_NORMAL);

    // prepare PAGE1
    PrintModePage2 = false;
    printXY(40-13,2, "A2DVI Test: 80 column mode", PRINTMODE_NORMAL);
    for (uint x=0;x<80;x++)
    {
        char s[3];
        if ((x&0xf)<10)
            s[0] = '0'+(x&0xf);
        else
            s[0] = 'A'+(x&0xf)-10;
        s[1] = 0;
        printXY(x,0, s, PRINTMODE_NORMAL);
        if (x<24)
            printXY(0,x, s, PRINTMODE_NORMAL);
    }
    for (uint i=0;i<255;i++)
    {
        testPrintChar((40-8)+(i&0xf), 4+(i>>4), i);
    }

    sleep(TestDelayMilliseconds);

    togglePages();                           // test both pages
    toggleAltChar();                         // test mouse text

    simulateWrite(REG_SW_40COL, 0);          // disable 80column mode
    PrintMode80Column = false;
#endif
}

void testLores()
{
#ifdef TEST_LORES
    simulateWrite(REG_SW_TEXT_OFF, 0);       // enable LORES graphics

    setLoresTestPattern(48);
    sleep(TestDelayMilliseconds);
    togglePages();                           // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0x80);  // enable MONOCHROME mode
    sleep(TestDelayMilliseconds);
    togglePages();                           // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0);     // disable MONOCHROME mode
    simulateWrite(REG_SW_TEXT, 0);           // enable text mode
#endif
}

void testLoresMix40()
{
#if (defined TEST_MIX_MODES) && ((defined TEST_40_COLUMNS)||(defined TEST_LORES))
    clearBothPages();
    simulateWrite(REG_SW_TEXT_OFF, 0);       // enable LORES graphics
    simulateWrite(REG_SW_MIX, 0);            // enable MIX MODE

    setLoresTestPattern(48-4*2);
    printXY(20-15,20, "A2DVI Test: LORES MIX MODE 40", PRINTMODE_NORMAL);
    printXY(20-10,23, "A2DVI TEST: FLASHING", PRINTMODE_FLASH);
    // prepare PAGE2
    PrintModePage2 = true;
    printXY(20-3,20, "PAGE 2", PRINTMODE_NORMAL);
    PrintModePage2 = false;

    sleep(TestDelayMilliseconds);
    togglePages();                           // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0x80);  // enable MONOCHROME mode
    sleep(TestDelayMilliseconds);
    togglePages();                           // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0);     // disable MONOCHROME mode
    simulateWrite(REG_SW_MIX_OFF,    0);     // disable MIX MODE
    simulateWrite(REG_SW_TEXT,       0);     // enable text mode
#endif
}

void testLoresMix80()
{
#if (defined TEST_MIX_MODES) && ((defined TEST_80_COLUMNS)||(defined TEST_80_COLUMNS))
    PrintMode80Column = true;
    simulateWrite(REG_SW_80COL, 0);         // enable 80column mode
    clearBothPages();
    simulateWrite(REG_SW_TEXT_OFF, 0);      // enable LORES graphics
    simulateWrite(REG_SW_MIX,   0);         // enable MIX MODE

    setLoresTestPattern(48-4*2);
    printXY(40-15,20, "A2DVI Test: LORES MIX MODE 80", PRINTMODE_NORMAL);
    printXY(40-10,23, "A2DVI TEST: FLASHING", PRINTMODE_FLASH);
    // prepare PAGE2
    PrintModePage2 = true;
    printXY(40-3,20, "PAGE 2", PRINTMODE_NORMAL);
    PrintModePage2 = false;

    sleep(TestDelayMilliseconds);
    togglePages();                          // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0x80); // enable MONOCHROME mode
    sleep(TestDelayMilliseconds);
    togglePages();                          // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0);    // disable MONOCHROME mode
    simulateWrite(REG_SW_MIX_OFF,    0);    // disable MIX MODE
    simulateWrite(REG_SW_TEXT,       0);    // enable text mode
    simulateWrite(REG_SW_40COL,      0);    // disable 80column mode
    PrintMode80Column = false;
#endif
}

void testDoubleLores()
{
#ifdef TEST_DOUBLE_LORES
    simulateWrite(REG_SW_TEXT_OFF, 0);      // enable LORES graphics
    simulateWrite(REG_SW_DGR,   0);         // enable double LORES

    setLoresTestPattern(48);
    sleep(TestDelayMilliseconds);

    simulateWrite(REG_SW_MONOCHROME, 0x80); // enable MONOCHROME mode
    sleep(TestDelayMilliseconds);

    simulateWrite(REG_SW_MONOCHROME, 0);    // disable MONOCHROME mode
    simulateWrite(REG_SW_DGR_OFF,    0);    // disable double LORES
    simulateWrite(REG_SW_TEXT,       0);    // enable text mode
#endif
}

void testHires()
{
#ifdef TEST_HIRES
    simulateWrite(REG_SW_HIRES, 0);         // enable HIRES graphics
    simulateWrite(REG_SW_TEXT_OFF, 0);      // disable TEXT mode

    setHiresTestPattern();
    sleep(TestDelayMilliseconds);

    togglePages();                          // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0x80); // enable MONOCHROME mode
    sleep(TestDelayMilliseconds);

    togglePages();                          // test both pages

    simulateWrite(REG_SW_MONOCHROME, 0);    // disable MONOCHROME mode
    simulateWrite(REG_SW_HIRES_OFF,  0);    // disable HIRES
    simulateWrite(REG_SW_TEXT,       0);    // enable text mode
#endif
}

void test_config()
{
    simulateWrite(REG_CARD+0x4, 0); // load defaults

    simulateWrite(REG_CARD+0x0, 1); // enable scanline emulation
    simulateWrite(REG_CARD+0x1, 3); // set color mode to amber

    simulateWrite(REG_CARD+0x8, 5); // select german character set as main
    simulateWrite(REG_CARD+0x9, 0); // select US enhanced as alternate

    simulateWrite(REG_CARD+0x4, 2); // save config
    simulateWrite(REG_CARD+0x4, 1); // load config
}

void test_menu()
{
#ifdef TEST_MENU
    showMenu('M');
    sleep(TestDelayMilliseconds);

    for (uint i=0;i<4;i++)
    {
        showMenu(8);
        sleep(TestDelayMilliseconds/3);
    }
#endif

    showTitle(PRINTMODE_INVERSE);
    menuShowDebug();
    sleep(TestDelayMilliseconds);
}

void test_loop()
{
    // initialize the Apple II bus interface
    abus_init();

#if 0
    test_config();
#else
    simulateWrite(REG_CARD+0x8, 5); // select german character set as main
    simulateWrite(REG_CARD+0x9, 0); // select US enhanced as alternate
#endif

    while (1)
    {
        current_machine = MACHINE_IIE;
        internal_flags |= IFLAGS_IIE_REGS;

        // test text modes
        test40columns();
        test80columns();

        test_menu();

        // text lo resolution graphics
        testLores();
        testDoubleLores();

        // test mix modes
        testLoresMix40();
        testLoresMix80();

        // test hires modes
        testHires();

        // toggle scanline emulation mode
        if (internal_flags & IFLAGS_SCANLINEEMU)
            simulateWrite(REG_CARD+0x0, 2);
        else
            simulateWrite(REG_CARD+0x0, 1);

        language_switch = !language_switch;

        // config setting is 1,2,3 for color_mode=0,1,2...
        simulateWrite(REG_CARD+0x1, (color_mode >= 2) ? 1 : (color_mode+2));
    }
}

#endif // FEATURE_TEST
