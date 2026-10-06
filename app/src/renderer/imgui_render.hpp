#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace Render {
class ImGuiManager {
public:
    static ImGuiManager& Get() {
        static ImGuiManager instance;
        return instance;
    }

    ImGuiManager(){};
    ~ImGuiManager(){};

    void Init();
    void Update();
    
protected:
    void BeforeRenderUpdate();
    void AfterRenderUpdate();
    void Render();
    void RenderImpl();

    void RenderDeviceBar();
};
}