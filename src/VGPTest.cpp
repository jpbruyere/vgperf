#include "VGPTest.hpp"
#include "Result.hpp"

#include <iostream>
#include <filesystem>

void VGPTest::initTest(VGPerfApp* app) {
    this->app = app;
    ctx = this->app->GetBackend();
    ctx->InitTest();
}
void VGPTest::cleanTest(double _totTime, double* run_time_values) {
    ctx->CleanUpTest();

    if (app->save_img) {    
        std::string path = ctx->name + "_" + this->name + ".png";
        if (!app->out_dir.empty()) {
            path = std::filesystem::path(app->out_dir) / path;
        }

        ctx->SaveImg(path);
    }
    Result r(this->app->iterations, _totTime, run_time_values);
    r.Print(std::cout, this->index, ctx->name, this->name);
}
int VGPTest::performTest(int inst) {
    this->run();
    return inst;
}