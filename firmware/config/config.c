/*
MIT License

Copyright (c) 2021 Mark Aikens
Copyright (c) 2023 David Kuder
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
#include <stdlib.h>
#include <string.h>
#include <hardware/flash.h>

#include "config.h"
#include "applebus/buffers.h"
#include "util/dmacopy.h"
#include "fonts/textfont.h"

volatile compat_t detected_machine = MACHINE_AUTO;
volatile compat_t cfg_machine = MACHINE_AUTO;
volatile compat_t current_machine = MACHINE_AUTO;
volatile bool language_switch_enabled = false; // true: language switch enabled/not ignored, false: language switch disabled/ignored
volatile bool language_switch = false; // false: main/local char set, true: alternate char set (normally fixed to US default)
uint8_t cfg_local_charset = 0;
uint8_t cfg_alt_charset = 0;
uint8_t reload_charsets = 0;
volatile uint8_t color_mode = 1;

// A block of flash is reserved for storing configuration persistently across power cycles
// and firmware updates.
//
// The memory is divided as:
//  * 4K for a 'config' structure
//  * the remaining is reserved for future use

// A2DV(I)
#define MAGIC_WORD_VALUE 0x41324456

// packed config structure (packed = do not padd to a multiple of 4 bytes)
struct __attribute__((__packed__)) config
{
    // magic word determines if the stored configuration is valid
    uint32_t magic_word;

    // the real size of the stored structure
    uint16_t size;
    uint8_t  scanline_emulation;
    uint8_t  forced_monochrome;

    uint8_t  color_mode;
    uint8_t  machine_type;
    uint8_t  local_charset; // selection for local language video ROM
    uint8_t  alt_charset;   // selection for alternate video ROM (usually fixed to US charset)

    uint8_t  custom_char_rom[CHARACTER_ROM_SIZE];

    uint8_t  language_switch_enabled;
    uint8_t  video7_enabled;

    // Add new fields after here. When reading the config use the IS_STORED_IN_CONFIG macro
    // to determine if the field you're looking for is actually present in the stored config.
};

// This is a compile-time check to ensure the size of the config struct fits within one flash erase sector
typedef char config_struct_size_check[(sizeof(struct config) <= FLASH_SECTOR_SIZE) - 1];

#define IS_STORED_IN_CONFIG(cfg, field) ((offsetof(struct config, field) + sizeof((cfg)->field)) <= (cfg)->size)


extern uint8_t __persistent_data_start[];
static struct config *cfg = (struct config *)__persistent_data_start;
// TODO static uint8_t *character_rom_storage = __persistent_data_start + FLASH_SECTOR_SIZE;

void __time_critical_func(set_machine)(compat_t machine)
{
    switch(machine)
    {
        case MACHINE_AUTO:
        case MACHINE_AGAT7:
        case MACHINE_AGAT9:
        case MACHINE_BASIS:
        case MACHINE_PRAVETZ:
        case MACHINE_II:
            internal_flags &= ~(IFLAGS_IIGS_REGS||IFLAGS_IIE_REGS);
            break;

        case MACHINE_IIE:
            internal_flags &= ~IFLAGS_IIGS_REGS;
            internal_flags |= IFLAGS_IIE_REGS;
            break;

        case MACHINE_IIGS:
            internal_flags &= ~IFLAGS_IIE_REGS;
            internal_flags |= IFLAGS_IIGS_REGS;
            break;

        default:
            break;
    }
    current_machine = machine;
}

void config_load_charsets(void)
{
    if (reload_charsets & 1)
    {
        // local font
        memcpy32(character_rom, character_roms[cfg_local_charset], CHARACTER_ROM_SIZE);
    }

    if (reload_charsets & 2)
    {
        // alternate fixed US font (with language switch)
        memcpy32(&character_rom[0x800], character_roms[cfg_alt_charset], CHARACTER_ROM_SIZE);
    }

    reload_charsets = 0;
}

void config_load(void)
{
    if((cfg->magic_word != MAGIC_WORD_VALUE) || (cfg->size > FLASH_SECTOR_SIZE))
    {
        config_load_defaults();
        return;
    }

    cfg_machine = (cfg->machine_type <= MACHINE_MAX_CFG) ? cfg->machine_type : MACHINE_AUTO;
    set_machine(cfg_machine);

    SET_IFLAG(cfg->scanline_emulation, IFLAGS_SCANLINEEMU);
    SET_IFLAG(cfg->forced_monochrome,  IFLAGS_FORCED_MONO);
    SET_IFLAG(cfg->video7_enabled,     IFLAGS_VIDEO7);

    language_switch_enabled = (cfg->language_switch_enabled != 0);

    color_mode = (cfg->color_mode <= 2) ? cfg->color_mode : 0;

    cfg_local_charset = cfg->local_charset;
    if (cfg_local_charset >= MAX_FONT_COUNT)
        cfg_local_charset = 0;

    cfg_alt_charset = cfg->alt_charset;
    if (cfg_alt_charset >= MAX_FONT_COUNT)
        cfg_alt_charset = 0;

    // load both character sets
    reload_charsets = 3;
    config_load_charsets();

#ifdef APPLE_MODEL_IIPLUS
    if(IS_STORED_IN_CONFIG(cfg, videx_vterm_enabled) && cfg->videx_vterm_enabled) {
        videx_vterm_enable();
    } else {
        videx_vterm_disable();
    }
#endif
}


void config_load_defaults(void)
{
    SET_IFLAG(1, IFLAGS_SCANLINEEMU);
    SET_IFLAG(0, IFLAGS_FORCED_MONO);
    SET_IFLAG(0, IFLAGS_VIDEO7);

    color_mode              = COLOR_MODE_GREEN;
    cfg_machine             = MACHINE_AUTO;
    set_machine(detected_machine);

    language_switch_enabled = true;

    cfg_local_charset       = DEFAULT_LOCAL_CHARSET;
    cfg_alt_charset         = DEFAULT_ALT_CHARSET;

    memcpy32(&character_rom[0],     character_roms[cfg_local_charset], CHARACTER_ROM_SIZE);
    memcpy32(&character_rom[0x800], character_roms[cfg_alt_charset],   CHARACTER_ROM_SIZE);

#ifdef APPLE_MODEL_IIPLUS
    videx_vterm_disable();
#endif
}


void config_save(void)
{
    // the write buffer size must be a multiple of FLASH_PAGE_SIZE so round up
    const int new_config_size = (sizeof(struct config) + FLASH_PAGE_SIZE - 1) & -FLASH_PAGE_SIZE;
    struct config *new_config = malloc(new_config_size);
    memset(new_config, 0xff, new_config_size);
    memset(new_config, 0, sizeof(struct config));

    // prepare header
    new_config->magic_word = MAGIC_WORD_VALUE;
    new_config->size       = sizeof(struct config);

    // set config properties
    new_config->scanline_emulation      = IS_IFLAG(IFLAGS_SCANLINEEMU);
    new_config->forced_monochrome       = IS_IFLAG(IFLAGS_FORCED_MONO);
    new_config->video7_enabled          = IS_IFLAG(IFLAGS_VIDEO7);
    new_config->color_mode              = color_mode;
    new_config->machine_type            = cfg_machine;
    new_config->local_charset           = cfg_local_charset;
    new_config->alt_charset             = cfg_alt_charset;
    new_config->language_switch_enabled = language_switch_enabled;

    //memcpy32(new_config->character_rom, character_rom, CHARACTER_ROM_SIZE);
#ifdef APPLE_MODEL_IIPLUS
    new_config->videx_vterm_enabled = videx_vterm_enabled;
#endif

    const uint32_t flash_offset = (uint32_t)cfg - XIP_BASE;
    flash_range_erase(flash_offset, FLASH_SECTOR_SIZE);
    flash_range_program(flash_offset, (uint8_t *)new_config, new_config_size);

    free(new_config);
}
