#pragma once

#include "VGPTest.hpp"
#include "ir/prepared_scene.h"

class VGPSceneTest : public VGPTest {
    vgcpu::PreparedScene scene;
  public:
    VGPSceneTest(vgcpu::PreparedScene _scene, std::string _name) : VGPTest(std::move(_name)), scene(std::move(_scene)) {}
    void run() override;
    void BuildPath(const vgcpu::Path& path);
};
