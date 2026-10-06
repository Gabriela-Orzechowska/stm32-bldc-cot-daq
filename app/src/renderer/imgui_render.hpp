#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../usb/usb_device.hpp"
#include <GLFW/glfw3.h>

namespace Render {
class ImGuiManager {
public:
    static ImGuiManager& Get() {
        static ImGuiManager instance;
        return instance;
    }

    ImGuiManager(){};
    ~ImGuiManager() {
        if (this->m_device) delete this->m_device;
    };

    void Init();
    void Update();
    
protected:
    void BeforeRenderUpdate();
    void AfterRenderUpdate();
    void Render();
    void RenderImpl();

    void RenderDeviceBar();

    USB::Device *m_device;
    bool m_isConnected = false;
};
}