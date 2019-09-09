/*
 * Copyright (c) 2018 Jean-Philippe Bruyère <jp_bruyere@hotmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "vkh.h"
#include "vkengine.h"
#include "vkh_app.h"
#include "vkh_phyinfo.h"
#include "vkh_presenter.h"
#include "vkh_image.h"
#include "vkh_device.h"

bool vkeCheckPhyPropBlitSource (VkEngine e) {
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(e->dev->phy, e->renderer->format, &formatProps);
    assert((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) && "Format cannot be used as blit source");
    assert((formatProps.optimalTilingFeatures & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) && "Format cannot be used as transfer source");
    assert((formatProps.optimalTilingFeatures & VK_IMAGE_USAGE_TRANSFER_DST_BIT) && "Format cannot be used as transfer dest");
    //VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT
}

VkSampleCountFlagBits getMaxUsableSampleCount(VkSampleCountFlags counts)
{
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
    return VK_SAMPLE_COUNT_1_BIT;
}

void vkengine_dump_Infos (VkEngine e){
    printf("max samples = %d\n", getMaxUsableSampleCount(e->gpu_props.limits.framebufferColorSampleCounts));
    printf("max tex2d size = %d\n", e->gpu_props.limits.maxImageDimension2D);
    printf("max tex array layers = %d\n", e->gpu_props.limits.maxImageArrayLayers);
    printf("max mem alloc count = %d\n", e->gpu_props.limits.maxMemoryAllocationCount);

    for (int i = 0; i < e->memory_properties.memoryHeapCount; i++) {
        printf("Mem Heap %d\n", i);
        printf("\tflags= %d\n", e->memory_properties.memoryHeaps[i].flags);
        printf("\tsize = %d Mo\n", e->memory_properties.memoryHeaps[i].size/ (1024*1024));
    }
    for (int i = 0; i < e->memory_properties.memoryTypeCount; i++) {
        printf("Mem type %d\n", i);
        printf("\theap %d: ", e->memory_properties.memoryTypes[i].heapIndex);
        if (e->memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            printf("VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|");
        if (e->memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            printf("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|");
        if (e->memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            printf("VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|");
        if (e->memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
            printf("VK_MEMORY_PROPERTY_HOST_CACHED_BIT|");
        if (e->memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
            printf("VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT|");
        printf("\n");
    }
}

static VkDebugReportCallbackEXT dbgReport;

vk_engine_t* vkengine_create (VkPhysicalDeviceType preferedGPU, VkPresentModeKHR presentMode, uint32_t width, uint32_t height) {
    vk_engine_t* e = (vk_engine_t*)calloc(1,sizeof(vk_engine_t));

    glfwInit();
    assert (glfwVulkanSupported()==GLFW_TRUE);

    uint32_t enabledExtsCount = 0, phyCount = 0;
    const char** gflwExts = glfwGetRequiredInstanceExtensions (&enabledExtsCount);

    const char* enabledExts [enabledExtsCount+2];

    for (uint i=0;i<enabledExtsCount;i++)
        enabledExts[i] = gflwExts[i];

#if defined (USE_VALIDATION)
    const uint32_t enabledLayersCount = 2;
    const char* enabledLayers[] = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_RENDERDOC_Capture"};
#else
    const uint32_t enabledLayersCount = 0;
    const char* enabledLayers[] = {NULL};
#endif
#if defined (DEBUG)
    enabledExts[enabledExtsCount] = "VK_EXT_debug_report";
    enabledExtsCount++;
    enabledExts[enabledExtsCount] = "VK_EXT_debug_utils";
    enabledExtsCount++;
#endif


    e->app = vkh_app_create("vkvgTest", enabledLayersCount, enabledLayers, enabledExtsCount, enabledExts);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING,   GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED,  GLFW_TRUE);

    e->window = glfwCreateWindow ((int)width, (int)height, "Window Title", NULL, NULL);
    VkSurfaceKHR surf;

    assert (glfwCreateWindowSurface(e->app->inst, e->window, NULL, &surf)==VK_SUCCESS);


    VkhPhyInfo* phys = vkh_app_get_phyinfos (e->app, &phyCount, surf);

    VkhPhyInfo pi = NULL;
    for (uint i=0; i<phyCount; i++){
        pi = phys[i];
        if (vkh_phyinfo_get_properties(pi).deviceType == preferedGPU)
            break;
    }

    e->memory_properties = vkh_phyinfo_get_memory_properties(pi);
    e->gpu_props = vkh_phyinfo_get_properties(pi);

    float queue_priorities[] = {0.0};
    VkDeviceQueueCreateInfo pQueueInfos[] = {{ .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                   .queueCount = 1,
                                   .queueFamilyIndex = vkh_phy_info_get_graphic_queue_index(pi),
                                   .pQueuePriorities = queue_priorities }};

#if defined(DEBUG) && defined(USE_VALIDATION)
    char const * dex [] = {"VK_KHR_swapchain", "VK_EXT_debug_marker"};
    enabledExtsCount = 2;
#else
    char const * dex [] = {"VK_KHR_swapchain"};
    enabledExtsCount = 1;
#endif


    VkPhysicalDeviceFeatures enabledFeatures = {
        .fillModeNonSolid = true,
        //.sampleRateShading = true
    };

    VkDeviceCreateInfo device_info = { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                       .queueCreateInfoCount = 1,
                                       .pQueueCreateInfos = (VkDeviceQueueCreateInfo*)&pQueueInfos,
                                       .enabledExtensionCount = enabledExtsCount,
                                       .ppEnabledExtensionNames = dex,
                                       .pEnabledFeatures = &enabledFeatures
                                     };

    e->dev = vkh_device_create (e->app, pi, &device_info);

#if DEBUG
    dbgReport = vkh_device_create_debug_report (e->dev,
    //VK_DEBUG_REPORT_INFORMATION_BIT_EXT|
            VK_DEBUG_REPORT_ERROR_BIT_EXT|
            VK_DEBUG_REPORT_WARNING_BIT_EXT|
            VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT|
            VK_DEBUG_REPORT_DEBUG_BIT_EXT);
#endif

    e->renderer = vkh_presenter_create
            (e->dev, (uint32_t) pi->pQueue, surf, width, height, VK_FORMAT_B8G8R8A8_UNORM, presentMode);

    vkh_app_free_phyinfos (phyCount, phys);

    return e;
}
inline void vkengine_wait_device_idle (VkEngine e) {
    vkDeviceWaitIdle(e->dev->dev);
}
void vkengine_destroy (VkEngine e) {
    vkDeviceWaitIdle(e->dev->dev);

    VkSurfaceKHR surf = e->renderer->surface;

    vkh_presenter_destroy (e->renderer);
    vkDestroySurfaceKHR (e->app->inst, surf, NULL);

    vkDestroyDevice (e->dev->dev, NULL);

    glfwDestroyWindow (e->window);
    glfwTerminate ();

    vkh_app_destroy (e->app);

    free(e);
}
void vkengine_close (VkEngine e) {
    glfwSetWindowShouldClose(e->window, GLFW_TRUE);
}
void vkengine_blitter_run (VkEngine e, VkImage img) {
    VkhPresenter p = e->renderer;
    vkh_presenter_build_blit_cmd (p, img, p->width, p->height);

    while (!vkengine_should_close (e)) {
        glfwPollEvents();
        if (!vkh_presenter_draw (p))
            vkh_presenter_build_blit_cmd (p, img, p->width, p->height);
    }
}
inline bool vkengine_should_close (VkEngine e) {
    return glfwWindowShouldClose (e->window);
}

VkDevice vkengine_get_device (VkEngine e){
    return e->dev->dev;
}
VkPhysicalDevice vkengine_get_physical_device (VkEngine e){
    return e->dev->phy;
}
VkInstance vkengine_get_instance (VkEngine e){
    return vkh_app_get_inst(e->app);
}
VkQueue vkengine_get_queue (VkEngine e){
    return e->renderer->queue;
}
uint32_t vkengine_get_queue_fam_idx (VkEngine e){
    return e->renderer->qFam;
}

void vkengine_set_key_callback (VkEngine e, GLFWkeyfun key_callback){
    glfwSetKeyCallback (e->window, key_callback);
}
void vkengine_set_mouse_but_callback (VkEngine e, GLFWmousebuttonfun onMouseBut){
    glfwSetMouseButtonCallback(e->window, onMouseBut);
}
void vkengine_set_cursor_pos_callback (VkEngine e, GLFWcursorposfun onMouseMove){
    glfwSetCursorPosCallback(e->window, onMouseMove);
}
void vkengine_set_scroll_callback (VkEngine e, GLFWscrollfun onScroll){
    glfwSetScrollCallback(e->window, onScroll);
}
void vkengine_set_char_callback (VkEngine e, GLFWcharfun onChar){
    glfwSetCharCallback(e->window, onChar);
}

