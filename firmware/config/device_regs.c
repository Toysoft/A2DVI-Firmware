/*
MIT License

Copyright (c) 2021 Mark Aikens
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

#include <stdint.h>
#include <string.h>

#include "config.h"
#include "device_regs.h"

#include "util/dmacopy.h"
#include "applebus/buffers.h"
#include "fonts/textfont.h"
#include "menu/menu.h"
#ifdef APPLE_MODEL_IIPLUS
#include "videx_vterm.h"
#endif


static unsigned int char_write_offset;
uint8_t dev_config_lock;

// Handle a write to one of the registers on this device's slot
void device_write(uint_fast8_t reg, uint_fast8_t data)
{
    if (reg == 0xf)
    {
        /* simple lock/version check to prevent issues with mismatching
         * configuration utilities. 11,22 has to be written to
         * enable the device config registers.
         * We can also use this in the future as a version check (check
         * if config utility matches). */
        switch(data)
        {
            case 11:
                dev_config_lock = 1;     // first step
                break;
            case 22:
                if (dev_config_lock == 1)
                    dev_config_lock = 2; // second step: unlocked
                break;
            default:
                dev_config_lock = 0;     // locked
                break;
        }
    }

    if (dev_config_lock != 2)
        return;

    switch(reg)
    {
    case 0x0:
        if(data & 0x01)
            SET_IFLAG(1, IFLAGS_SCANLINEEMU);
        if(data & 0x02)
            SET_IFLAG(0, IFLAGS_SCANLINEEMU);
#ifdef APPLE_MODEL_IIPLUS
        if(data & 0x04)
            videx_vterm_enable();
        if(data & 0x08)
            videx_vterm_disable();
#endif
        break;

    // soft-monochrome color setting
    case 0x1:
        if (data & 0x03)
            color_mode = (data & 0x3)-1;
        //0x30
        if(data & 0x40)
            SET_IFLAG(1, IFLAGS_FORCED_MONO);
        if(data & 0x80)
            SET_IFLAG(0, IFLAGS_FORCED_MONO);
        break;

    // character generator write offset
    case 0x2:
        char_write_offset = data << 3;
        break;

    // character generator write
    case 0x3:
        character_rom[char_write_offset] = data;
        char_write_offset = (char_write_offset + 1) % sizeof(character_rom);
        break;

    // device command
    case 0x4:
        execute_device_command(data);
        break;

    // set local/main character set
    case 0x5:
        if (data < MAX_FONT_COUNT)
        {
            // load a standard alternate character ROM
            cfg_local_charset = data;
            reload_charsets = 1;
        }
        break;

    // set alternate character set
    case 0x6:
        if (data < MAX_FONT_COUNT)
        {
            // load a standard alternate character ROM
            cfg_alt_charset = data;
            reload_charsets = 2;
            language_switch_enabled = true;
        }
        else
        {
            language_switch_enabled = false;
        }
        break;

    // set VIDEO7 (and other future) properties
    case 0x7:
        if(data & 0x1)
            SET_IFLAG(1, IFLAGS_VIDEO7);
        if(data & 0x2)
            SET_IFLAG(0, IFLAGS_VIDEO7);
        break;

    // configure machine type
    case 0x8:
        if (data < MACHINE_MAX_CFG)
            cfg_machine = data;
        break;

    case 0x9:
        if (data == 0)
        {
            // 0 unlocks the menu dialog
            SET_IFLAG(1, IFLAGS_MENU_ENABLE);
        }
        if (IS_IFLAG(IFLAGS_MENU_ENABLE))
        {
            // show/update menu dialog
            menuShow(data);
        }
        break;

    default:
        break;
    }
}


// Handle a write to the "command" register to perform some one-shot action based on the
// command value.
//
// Note: some of these commands could take a long time (relative to 6502 bus cycles) so
// some bus activity may be missed. Other projects like the V2-Analog delegate this execution
// to the other (VGA) core to avoid this. Maybe do this if the missed bus cycles become a noticable
// issue; I only expect it would happen when some config is being saved, which is not done often.
void execute_device_command(uint_fast8_t cmd)
{
    switch(cmd)
    {
        case 0x00:
            // reset to the default configuration
            config_load_defaults();
            break;
        case 0x01:
            // reset to the saved configuration
            config_load();
            break;
        case 0x02:
            // save the current configuration
            config_save();
            break;
        case 0x10 ... (0x10+MAX_FONT_COUNT):
            // load a standard alternate character ROM
            cfg_local_charset = cmd - 0x10;
            reload_charsets = 1;
            break;
        default:
            break;
    }
}
