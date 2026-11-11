#include "VGPerfApp.hpp"
#include "VGPTest.hpp"

#ifdef ENABLE_VKVG
#include "backends/Vkvg.hpp"
#endif
#ifdef ENABLE_CAIRO
#include "backends/Cairo.hpp"
#endif

#include <chrono>

void VGPerfApp::Init() {
#ifdef ENABLE_VKVG
    backends.push_back(new VkvgRenderContext());
#endif
#ifdef ENABLE_VKVG
    backends.push_back(new CairoRenderContext());
#endif
}
void VGPerfApp::Run() {
    Init();

    if (VGPTest::tests.empty())
        return;

    for (curBackendIdx = 0; curBackendIdx < backends.size(); ++curBackendIdx) {
        uint32_t curIter = 0;

        int testIdx = 0;
        auto it = VGPTest::tests.begin();
        VGPTest* curTest;
        std::vector<double> run_time_values;
        run_time_values.reserve(iterations);
        double totTime = 0;

        if (testsToRun.empty()) {
            curTest = it->get();
        } else if (testsToRun[testIdx] < VGPTest::tests.size()){
            curTest = VGPTest::tests[testsToRun[testIdx]].get();
        } else {
            printf("test index out of bounds: %d\n", testsToRun[testIdx]);
            continue;
        }
        GetBackend()->Init(this);
        curTest->initTest(this);
        curTest->performTest(testIdx);//warm-up

        while (true) {
            int idx = testIdx;
            auto start = std::chrono::steady_clock::now();
            DoNotOptimize(idx);
            int res = curTest->performTest(idx);
            DoNotOptimize(res);
            auto finish = std::chrono::steady_clock::now();

            auto elapsed = finish - start;
            /*double elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                         elapsed).count();*/
            double elapsed_ms = std::chrono::duration<double, std::milli>(elapsed).count();

            if (iterations > 0) {
                totTime += elapsed_ms;
                run_time_values.push_back(elapsed_ms);

                if (++curIter == iterations) {
                    curTest->cleanTest(totTime, run_time_values.data());
                    testIdx++;
                    if (testsToRun.empty()) {
                        if (++it == VGPTest::tests.end()) {
                            curTest = NULL;
                            break;
                        }
                        curTest = it->get();
                    } else {
                        if (testIdx == (int)testsToRun.size()) {
                            curTest = NULL;
                            break;
                        }
                        curTest = VGPTest::tests[testsToRun[testIdx]].get();
                    }

                    curIter = 0;
                    totTime = 0;
                    run_time_values.clear();
                    run_time_values.reserve(iterations);

                    curTest->initTest(this);
                    curTest->performTest(testIdx);//warm-up
                }
            }
        }
        if (curTest != NULL)
            curTest->cleanTest(totTime, run_time_values.data());

        GetBackend()->CleanUp();
    }

    CleanUp();
}
void VGPerfApp::CleanUp() {
}



