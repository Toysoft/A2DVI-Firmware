#pragma once

#include "dvi.h"
#include "dvi/tmds.h"

extern struct dvi_inst dvi0;

#define DVI_WORDS_PER_CHANNEL (640/2)
#define DVI_APPLE2_XOFS       ((640/2-560/2)/2)

#define dvi_get_scanline(tmdsbuf)  \
    uint32_t* tmdsbuf;\
    queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);

#define dvi_scanline_rgb(tmdsbuf, tmdsbuf_red, tmdsbuf_green, tmdsbuf_blue) \
        uint32_t *tmdsbuf_blue  = tmdsbuf+DVI_APPLE2_XOFS; \
        uint32_t* tmdsbuf_green = tmdsbuf_blue  + DVI_WORDS_PER_CHANNEL; \
        uint32_t* tmdsbuf_red   = tmdsbuf_green + DVI_WORDS_PER_CHANNEL;

#define dvi_copy_scanline(destbuf, srcbuf) \
    for (uint32_t i=0;i<DVI_WORDS_PER_CHANNEL-DVI_APPLE2_XOFS;i++) \
    { \
        destbuf[i                        ] = srcbuf[i                        ]; \
        destbuf[i+  DVI_WORDS_PER_CHANNEL] = srcbuf[i+  DVI_WORDS_PER_CHANNEL]; \
        destbuf[i+2*DVI_WORDS_PER_CHANNEL] = srcbuf[i+2*DVI_WORDS_PER_CHANNEL]; \
    }

#define dvi_send_scanline(tmdsbuf) \
    queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);

extern bool mono_rendering;

extern void render_loop();

extern void update_text_flasher();
extern void render_text();
extern void render_mixed_text();

extern void render_lores();
extern void render_mixed_lores();

extern void render_hires();
extern void render_mixed_hires();

extern void render_dhgr();
extern void render_mixed_dhgr();

extern void render_dgr();
extern void render_mixed_dgr();
