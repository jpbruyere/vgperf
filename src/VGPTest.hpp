#pragma once

#include <string>
#include <memory>

#include "VGPerfApp.hpp"

class VGPTest {
    double totTime;
    double avg_run_time;
    double med_run_time;
    double standard_dev;
    double avg_frames_per_second;
  protected:
    std::string name;
  public:
    virtual ~VGPTest() = default;
    virtual void run() = 0;
    int performTest(int inst);
    void initTest(VGPerfApp* app);
    void cleanTest(double _totTime, double *run_time_values);
    const std::string& getName() const noexcept { return name; }

    VGPTest(std::string _name) : name(std::move(_name)) {}

    static inline std::vector<std::unique_ptr<VGPTest>> tests;
    static void registerTest(std::unique_ptr<VGPTest> test) {
        test->index = tests.size();
        tests.push_back(std::move(test));
    }
    int index;
    VGPerfApp *app;
    RenderContext *ctx;

};