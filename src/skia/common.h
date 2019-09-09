#ifndef SKIA_COMMON_H
#define SKIA_COMMON_H

#include "../vgperf.h"

#include "../vkvg/vkengine.h"

#include "include/c/gr_context.h"
#include "include/c/sk_canvas.h"
#include "include/c/sk_data.h"
#include "include/c/sk_image.h"
#include "include/c/sk_imageinfo.h"
#include "include/c/sk_paint.h"
#include "include/c/sk_path.h"
#include "include/c/sk_surface.h"


typedef struct _library_context {
    VkEngine vke;
    gr_context_t* gr;
    sk_surface_t* surf;
    gr_vk_image_info_t* vkImgInfo;
    sk_canvas_t* ctx;
    sk_paint_t* paint;
} library_context_t;

#endif
