
#include "common.h"
#include "vkh_device.h"
#include "vkh_presenter.h"

#include "line.h"
#include "rect.h"
#include "rectangles.h"

void randomize_color (VkvgContext ctx) {
    vkvg_set_source_rgba(ctx,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX,
        (float)rnd()/RAND_MAX
    );
}

/**
 * @brief init library once at startup
 * @param global options
 * @return a pointer to the library test context
 */
library_context_t* initLibrary(options_t* opt) {
    library_context_t* ctx = (library_context_t*)calloc(1, sizeof(library_context_t));

    ctx->vkEngine = vkengine_create (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, opt->width, opt->height);
    VkhPresenter r = ctx->vkEngine->renderer;

    ctx->dev = vkvg_device_create (r->dev->phy, r->dev->dev, r->qFam, 0);

    ctx->surf = vkvg_surface_create (ctx->dev, opt->width, opt->height);

    return ctx;
}
/**
 * @brief cleanup Library and release test context pointer
 * @param library test context
 */
void cleanupLibrary (library_context_t* ctx) {
    vkvg_surface_destroy(ctx->surf);

    vkvg_device_destroy (ctx->dev);

    vkengine_destroy (ctx->vkEngine);

    free(ctx);
}

void init_vkvg_tests (test_context_t* ctx) {
    ctx->libName = "vkvg";
    ctx->init = initLibrary;
    ctx->cleanup = cleanupLibrary;

    ctx->tests [TID_lines].init = line_init;
    ctx->tests [TID_lines].perform = line_perform;
    ctx->tests [TID_lines].cleanup = line_cleanup;

    ctx->tests [TID_rects].init = rect_init;
    ctx->tests [TID_rects].perform = rect_perform;
    ctx->tests [TID_rects].cleanup = rect_cleanup;

    ctx->tests [TID_rectangles].init = rectangles_init;
    ctx->tests [TID_rectangles].perform = rectangles_perform;
    ctx->tests [TID_rectangles].cleanup = rectangles_cleanup;

}
