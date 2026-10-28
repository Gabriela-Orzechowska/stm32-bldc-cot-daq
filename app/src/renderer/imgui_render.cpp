#include "imgui_render.hpp"
#include <iostream>
#include "common/packet.h"
#include "tcp/tcp.hpp"
#include <vendor/crc.h>
#include "device/manager.hpp"
#include <optional>
#include <string>
#include <filesystem>
#include <nfd.h>
#include <imgui_internal.h>

namespace Render {

static std::optional<std::filesystem::path> _OpenFolder() {
    nfdchar_t* outPath = nullptr;

    nfdresult_t ret = NFD_PickFolderU8(&outPath, nullptr);

    if (ret != NFD_OKAY) return std::nullopt;

    std::filesystem::path path(outPath);
    NFD_FreePathU8(outPath);

    return path;
}

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

    Device::Manager& device = Device::Manager::Get();

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
    if (TCP::ClientManager::Get().IsConnected()) {
        ImGui::PushItemWidth(140.0f);
        ImGui::Text("Device Info: Type: %s, Timer Clock: %uMHz", device.GetDeviceName().c_str(), device.GetDeviceClock() / 1000000);


		config_t& config = device.GetConfig();
        int configResolution = config.encoder_resolution;
        int configMode = config.encoder_mode;
        int configChannel = config.encoder_channel;

        
        ImGui::InputInt("Encoder Resolution", &configResolution, 1);
        ImGui::SameLine();
        ImGui::Combo("Mode", &configMode, "Single edge\0Both edges\0XOR\0\0");
        if (configMode != 2) {
            ImGui::SameLine();
            ImGui::Combo("Channel", &configChannel, "A\0B\0\0");
        }
        config.encoder_resolution = configResolution;
        config.encoder_mode = configMode;
        config.encoder_channel = configChannel;

        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            device.SendConfig();
        }


        if (ImGui::Button("Select Output Folder") && !device.m_isCapturing) {
            auto ret = _OpenFolder();
            if (ret.has_value()) {
                device.m_outputPath = ret.value();
                device.UpdatePaths();
            }
        }
        ImGui::SameLine();
        ImGui::Text("%ls", device.m_outputPath.c_str());

        if (device.m_outputPath != "") {
            if (!device.m_isCapturing && ImGui::Button("Start Capture")) {
                device.BeginCapture();
            } else if (device.m_isCapturing && ImGui::Button("Stop Capture")) {
                device.EndCapture();
            }
        }
    }


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
