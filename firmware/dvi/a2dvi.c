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

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "a2dvi.h"
#include "dvi.h"
#include "dvi_pin_config.h"
#include "dvi_serialiser.h"
#include "dvi_timing.h"
#include "render/render.h"
#include "util/dmacopy.h"
#include "config/config.h"

// clock/DVI configuration
#define DVI_TIMING        dvi_timing_640x480p_60hz
#define DVI_SERIAL_CONFIG pico_a2dvi_cfg

struct dvi_inst dvi0;

static void a2dvi_init()
{
    // wait a bit, until the raised core VCC has settled
    sleep_ms(5);
    // shift into higher gears...
    set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);
}

void DELAYED_COPY_CODE(a2dvi_loop)()
{
    // free DMA channel and stop others from using it (would interfere with the DVI processing)
    dmacopy_disable_dma();

    // CPU clock configuration required for DVI
    a2dvi_init();

    // configure DVI
    dvi0.timing = &DVI_TIMING;
    dvi0.ser_cfg = DVI_SERIAL_CONFIG;
    dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());
    dvi0.scanline_emulation = true;
    dvi_register_irqs_this_core(&dvi0, DMA_IRQ_0);
    dvi_start(&dvi0);

    // start DVI output
    render_loop();

    __builtin_unreachable();
}
