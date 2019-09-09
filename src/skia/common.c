
#include "common.h"
#include "basic_tests.h"

void skia_present (options_t* opt, library_context_t* ctx) {
    gr_context_flush(ctx->gr);
    VkhPresenter r = ctx->vke->renderer;
    if (!vkh_presenter_draw (r))
        vkh_presenter_build_blit_cmd (r, gr_vk_image_info_get_image(ctx->vkImgInfo), (uint)opt->width, (uint)opt->height);

    vkengine_wait_device_idle(ctx->vke);
}

library_context_t* skia_initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    ctx->vke = vkengine_create (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PRESENT_MODE_MAILBOX_KHR,
                                (uint)opt->width, (uint)opt->height);
    VkhPresenter r = ctx->vke->renderer;

    ctx->gr = gr_context_new_vulkan(
                vkengine_get_instance(ctx->vke), vkengine_get_physical_device(ctx->vke),
                vkengine_get_device(ctx->vke), vkengine_get_queue(ctx->vke));


    sk_imageinfo_t* info = sk_imageinfo_new(opt->width, opt->height, RGBA_8888_SK_COLORTYPE,
                                            PREMUL_SK_ALPHATYPE, NULL);


    ctx->surf = sk_surface_make_rendertarget(ctx->gr, info);

    ctx->vkImgInfo = sk_surface_get_vk_image_info(ctx->surf);

    if (opt->present == 1)
        vkh_presenter_build_blit_cmd (r, gr_vk_image_info_get_image(ctx->vkImgInfo), (uint)opt->width, (uint)opt->height);

    return ctx;
}
void skia_cleanupLibrary (library_context_t* ctx) {
    gr_vk_image_info_delete(ctx->vkImgInfo);
    sk_surface_unref(ctx->surf);
    gr_context_unref(ctx->gr);
    free(ctx);
}
void skia_initTest(options_t* opt, library_context_t* ctx) {
    ctx->ctx = sk_surface_get_canvas(ctx->surf);
    ctx->paint = sk_paint_new();
    sk_paint_set_antialias(ctx->paint, true);
    //clear image
    sk_paint_t* fill = sk_paint_new();
    sk_paint_set_color(fill, sk_color_set_argb(0xFF, 0x00, 0x00, 0x00));
    sk_canvas_draw_paint(ctx->ctx, fill);
    sk_paint_delete(fill);

    //ctx->paint = sk_paint_new();
    /*vkvg_surface_clear(ctx->surf);

    //cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
    vkvg_set_line_width (ctx->ctx, opt->lineWidth);

    switch (opt->capStyle) {
    case LINE_CAP_BUTT:
        vkvg_set_line_cap(ctx->ctx, VKVG_LINE_CAP_BUTT);
        break;
    case LINE_CAP_ROUND:
        vkvg_set_line_cap(ctx->ctx, VKVG_LINE_CAP_ROUND);
        break;
    case LINE_CAP_SQUARE:
        vkvg_set_line_cap(ctx->ctx, VKVG_LINE_CAP_SQUARE);
        break;
    }
    switch (opt->joinStyle) {
    case LINE_JOIN_MITER:
        vkvg_set_line_join(ctx->ctx, VKVG_LINE_JOIN_MITER);
        break;
    case LINE_JOIN_BEVEL:
        vkvg_set_line_join(ctx->ctx, VKVG_LINE_JOIN_BEVEL);
        break;
    case LINE_JOIN_ROUND:
        vkvg_set_line_join(ctx->ctx, VKVG_LINE_JOIN_ROUND);
        break;
    }
    vkvg_set_fill_rule(ctx->ctx, VKVG_FILL_RULE_EVEN_ODD);*/

}
void skia_cleanupTest (options_t* opt, library_context_t* ctx) {
    sk_paint_delete(ctx->paint);
}
void skia_saveImg (library_context_t* ctx, const char* fileName) {
    sk_image_t* image = sk_surface_new_image_snapshot(ctx->surf);
    sk_data_t* data = sk_image_encode(image);
    sk_image_unref(image);
    FILE* f = fopen(fileName, "wb");
    fwrite(sk_data_get_data(data), sk_data_get_size(data), 1, f);
    fclose(f);
    sk_data_unref(data);
}

int init_skia_tests (vgperf_context_t** libs) {
    vgperf_context_t* ctx = (vgperf_context_t*)malloc(sizeof(vgperf_context_t));

    libs[0] = ctx;

    ctx->libName    = "skia";
    ctx->init       = (PFNinitLibrary) skia_initLibrary;
    ctx->cleanup    = (PFNcleanupLibrary) skia_cleanupLibrary;
    ctx->saveImg    = (PFNSaveImg) skia_saveImg;
    ctx->present    = (PFNtest) skia_present;

    ctx->testCount  = 0;
    ctx->tests      = (test_t*)malloc(0);

    addTest (ctx, "lines stroke", skia_initTest, skia_line_perform, skia_cleanupTest);
    addTest (ctx, "rectangles", skia_initTest, skia_rectangles_perform, skia_cleanupTest);
    addTest (ctx, "circles", skia_initTest, skia_circles_perform, skia_cleanupTest);
    addTest (ctx, "stars", skia_initTest, skia_stars_perform, skia_cleanupTest);
    //addTest (ctx, "test1", NULL, vkvg_test1_perform, NULL);
    //addTest(ctx, "single poly", initTest, single_poly_perform, cleanupTest);

    return 1;
}
