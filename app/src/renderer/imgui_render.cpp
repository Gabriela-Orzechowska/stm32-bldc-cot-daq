#include "imgui_render.hpp"
#include <iostream>
#include "../common/packet.h"
#include "tcp/tcp.hpp"
#include <vendor/crc.h>

namespace Render {


void ImGuiManager::Init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontDefaultVector();  // Load embedded scalable font.

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

    const char* status = "Disconnected";
    if (TCP::ClientManager::Get().IsConnected()) {
        status = "Connected";
    } else if (TCP::ClientManager::Get().IsConnecting()) {
        status = "Connecting...";
    }

    ImGui::Text("STM32 COT DAQ: %s", status);

    if (!TCP::ClientManager::Get().IsConnecting()) {
        if (TCP::ClientManager::Get().IsConnected()) {
            if (ImGui::Button("Disconnect")) {
                TCP::ClientManager::Get().TryDisconnect();
            }
        }
        else {
            if (ImGui::Button("Connect")) {
                TCP::ClientManager::Get().TryConnect();
            }
        }

    }

    /*
    if (USB::Manager::Get().IsConnected()) {
        config_t& config = USB::Manager::Get().GetConfig();
        int configResolution    = config.encoder_resolution;
        int configMode          = config.encoder_mode;
        int configChannel       = config.encoder_channel;

        ImGui::PushItemWidth(140.0f);
        ImGui::InputInt("Rozdzielczość enkodera", &configResolution, 1);
        ImGui::Combo("Tryb", &configMode, "Jedno zbocze\0Oba zbocza\0XOR\0\0");
        if (configMode != 2) {
            ImGui::Combo("Kanał", &configChannel, "A\0B\0\0");
        }
        config.encoder_resolution   = configResolution;
        config.encoder_mode         = configMode;
        config.encoder_channel      = configChannel;

        ImGui::PopItemWidth();
        if (ImGui::Button("Zapisz")) {
            USB::Manager::Get().SendConfig();
        }
    }
    */

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
