#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <vulkan/vulkan.h>

#include "RenderContext.hpp"

#if defined(__GNUC__) || defined(__clang__)
#define BENCHMARK_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER) && !defined(__clang__)
#define BENCHMARK_ALWAYS_INLINE __forceinline
#define __func__ __FUNCTION__
#else
#define BENCHMARK_ALWAYS_INLINE
#endif
template <class Tp>
inline BENCHMARK_ALWAYS_INLINE void DoNotOptimize(Tp& value) {
#if defined(__clang__)
    asm volatile("" : "+r,m"(value) : : "memory");
#else
    asm volatile("" : "+m,r"(value) : : "memory");
#endif
}

class VGPerfApp {
    void Init();
    void CleanUp();

  public:
    std::vector<RenderContext*> backends;
    std::vector<uint32_t>       testsToRun;

    uint32_t              width       = 1024;
    uint32_t              height      = 1024;
    uint32_t              iterations  = 200;
    uint32_t              testSize    = 200;
    std::vector<uint32_t> logSeverity = {1};
    uint32_t              logType     = 0;
    bool                  listGpus    = false;
    bool                  save_img    = false;
    std::string           out_dir;
    int32_t               gpuIndex    = -1;
    VkSampleCountFlags    samples     = VK_SAMPLE_COUNT_1_BIT;

    VkPhysicalDeviceType  preferedGPU = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    VkPresentModeKHR      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    uint32_t curBackendIdx;

    void Run();
    inline RenderContext* GetBackend() { return backends[curBackendIdx]; }
};
