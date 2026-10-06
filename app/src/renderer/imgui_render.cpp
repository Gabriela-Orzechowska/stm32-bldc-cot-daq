#include "imgui_render.hpp"

namespace Render {
void ImGuiManager::Init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
}

void ImGuiManager::Update() {
    this->BeforeRenderUpdate();
    this->Render();
    this->AfterRenderUpdate();
}

void ImGuiManager::BeforeRenderUpdate() {}
void ImGuiManager::AfterRenderUpdate() {}

void ImGuiManager::Render() {
    ImGui::NewFrame();
    this->RenderImpl();
    ImGui::Render();
}

void ImGuiManager::RenderDeviceBar() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0.0f,0.0f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 0.0f));

    ImGui::Begin("Device Bar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar
    );

    ImGui::Button("Test");

    ImGui::End();
}

void ImGuiManager::RenderImpl() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoBackground;
    
    ImGui::Begin("Root", nullptr, flags);

    this->RenderDeviceBar();

    ImGui::End();
}



}