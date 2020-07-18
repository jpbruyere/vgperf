
#include "common.h"
#include "vkh_device.h"
#include "vkh_presenter.h"

#include "basic_tests.h"

void vkvg_present (options_t* opt, library_context_t* ctx) {
    vkvg_flush (ctx->ctx);
    VkhPresenter r = ctx->vkEngine->renderer;
    if (!vkh_presenter_draw (r))
        vkh_presenter_build_blit_cmd (r, vkvg_surface_get_vk_image(ctx->surf), (uint)opt->width, (uint)opt->height);
    vkDeviceWaitIdle(r->dev->dev);
}

/**
 * @brief init library once at startup
 * @param global options
 * @return a pointer to the library test context
 */
library_context_t* vkvg_initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    ctx->vkEngine = vkengine_create (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PRESENT_MODE_MAILBOX_KHR, (uint)opt->width, (uint)opt->height);
    VkhPresenter r = ctx->vkEngine->renderer;

    switch (opt->antialias) {
    case ANTIALIAS_DEFAULT:
    case ANTIALIAS_NONE:
        ctx->dev = vkvg_device_create (vkh_app_get_inst(ctx->vkEngine->app), r->dev->phy, r->dev->dev, r->qFam, 0);
        break;
    case ANTIALIAS_FAST:
        ctx->dev = vkvg_device_create_multisample (vkh_app_get_inst(ctx->vkEngine->app), r->dev->phy, r->dev->dev, r->qFam, 0, VK_SAMPLE_COUNT_2_BIT, 0);
        break;
    case ANTIALIAS_GOOD:
        ctx->dev = vkvg_device_create_multisample (vkh_app_get_inst(ctx->vkEngine->app), r->dev->phy, r->dev->dev, r->qFam, 0, VK_SAMPLE_COUNT_4_BIT, 0);
        break;
    case ANTIALIAS_BEST:
        ctx->dev = vkvg_device_create_multisample (vkh_app_get_inst(ctx->vkEngine->app), r->dev->phy, r->dev->dev, r->qFam, 0,
                  vkengine_get_MaxUsableSampleCount(ctx->vkEngine->gpu_props.limits.framebufferColorSampleCounts), 0);
        break;
    }

    ctx->surf = vkvg_surface_create (ctx->dev, (uint)opt->width, (uint)opt->height);

    if (opt->present == 1)
        vkh_presenter_build_blit_cmd (r, vkvg_surface_get_vk_image(ctx->surf), (uint)opt->width, (uint)opt->height);

    return ctx;
}
/**
 * @brief cleanup Library and release test context pointer
 * @param library test context
 */
void vkvg_cleanupLibrary (library_context_t* ctx) {
    vkvg_surface_destroy(ctx->surf);

    vkvg_device_destroy (ctx->dev);

    vkengine_destroy (ctx->vkEngine);

    free(ctx);
}
/**
 * @brief clear surface and create and setup vkvg context, run before each test occurence
 * @param global options
 * @param library context
 */
void vkvg_initTest(options_t* opt, library_context_t* ctx) {
    //vkvg_surface_clear(ctx->surf);

    ctx->ctx = vkvg_create(ctx->surf);
    vkvg_set_source_rgba(ctx->ctx,0,0,0,1);
    vkvg_paint(ctx->ctx);

    //cairo_set_antialias (ctx->ctx, CAIRO_ANTIALIAS_NONE);
    vkvg_set_line_width (ctx->ctx, opt->lineWidth);

    /*switch (opt->capStyle) {
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
    vkvg_set_fill_rule(ctx->ctx, VKVG_FILL_RULE_NON_ZERO);*/

}
/**
 * @brief cleanup Test, present surface on screen if requested. run after each test occurence.
 * @param global options
 * @param library context
 */
void vkvg_cleanupTest (options_t* opt, library_context_t* ctx) {
    vkvg_destroy(ctx->ctx);
}
void vkvg_saveImg (library_context_t* ctx, const char* fileName) {
    vkvg_surface_write_to_png(ctx->surf, fileName);
}
/**
 * @brief set library func pointers and register available tests.
 * @param ctx
 * @return number of new vgperf context created
 */
int init_vkvg_tests (vgperf_context_t** libs) {
    vgperf_context_t* ctx = (vgperf_context_t*)malloc(sizeof(vgperf_context_t));

    libs[0] = ctx;

    ctx->libName    = "vkvg";
    ctx->init       = (PFNinitLibrary) vkvg_initLibrary;
    ctx->cleanup    = (PFNcleanupLibrary) vkvg_cleanupLibrary;
    ctx->saveImg    = (PFNSaveImg) vkvg_saveImg;
    ctx->present    = (PFNtest) vkvg_present;

    ctx->testCount  = 0;
    ctx->tests      = (test_t*)malloc(0);

    addTest (ctx, "lines stroke", vkvg_initTest, vkvg_line_perform, vkvg_cleanupTest);
    addTest (ctx, "rectangles", vkvg_initTest, vkvg_rectangles_perform, vkvg_cleanupTest);
    addTest (ctx, "circles", vkvg_initTest, vkvg_circles_perform, vkvg_cleanupTest);
    addTest (ctx, "stars", vkvg_initTest, vkvg_stars_perform, vkvg_cleanupTest);
    //addTest (ctx, "test1", NULL, vkvg_test1_perform, NULL);
    //addTest(ctx, "single poly", initTest, single_poly_perform, cleanupTest);

    return 1;
}
