#pragma once

#include "VGPTest.hpp"
#include <filesystem>

/*
#define M_PIF_MULT_2 6.28318530718f
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 // pi/2
#endif
*/
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif


namespace fs = std::filesystem;
#define GET_PATH(fileName) (char*)(fs::path(VGPERF_DATA_ROOT) / fileName).c_str()

typedef enum _shape_t {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_ROUNDED_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE,
    SHAPE_STAR,
    SHAPE_RANDOM,
} shape_t;

class VGPFuncTest : public VGPTest {
  public:
    VGPFuncTest(std::string _name) : VGPTest(std::move(_name)) {}
  protected:
    void SetRandomColor();
    void DrawShape(shape_t shape, float sizeFact);
};

#define VGP_CONCAT_IMPL(a, b) a ## b
#define VGP_CONCAT(a, b) VGP_CONCAT_IMPL(a, b)

#define TEST(test_name)                                                        \
class VGP_CONCAT(TestClass_, test_name) : public VGPFuncTest {                 \
  public:                                                                      \
    VGP_CONCAT(TestClass_, test_name)() : VGPFuncTest(#test_name) {}           \
    void run() override;                                                       \
};                                                                             \
static inline bool VGP_CONCAT(reg_, test_name) = []() {                        \
    VGPTest::registerTest(                                                     \
        std::make_unique<VGP_CONCAT(TestClass_, test_name)>()                  \
    );                                                                         \
    return true;                                                               \
}();                                                                           \
void VGP_CONCAT(TestClass_, test_name)::run()