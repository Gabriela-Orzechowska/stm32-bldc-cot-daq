#include "imgui_render.hpp"
#include "implot.h"
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
    ImPlot::CreateContext();

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

float ImGuiManager::RenderDeviceBar() {
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

        if (device.m_isCapturing)
            ImGui::BeginDisabled();

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

        if (ImGui::CollapsingHeader("ADC Configuration")) {
            for(int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                if (ImGui::BeginTabBar(std::format("ADC%d##adcconf", i+1).c_str())) {
                    auto& adcConf = config.adc_config[i];
                    ImGui::Checkbox("Enable Mapping", (bool*)&adcConf.enabled);
                    ImGui::SameLine();
                    if (!adcConf.enabled)
                        ImGui::BeginDisabled();
                    
                    ImGui::InputText("Unit", adcConf.unit, sizeof(adcConf.unit));

                    float maxVal = adcConf.max_val;
                    float minVal = adcConf.min_val;

                    ImGui::InputFloat("Max", &maxVal);
                    ImGui::InputFloat("Min", &minVal);

                    adcConf.max_val = maxVal;
                    adcConf.min_val = minVal;

                    if (!adcConf.enabled)
                        ImGui::EndDisabled();
                    ImGui::EndTabBar();
                }
            }

        }

        config.encoder_resolution = configResolution;
        config.encoder_mode = configMode;
        config.encoder_channel = configChannel;


        ImGui::PopItemWidth();
        if (ImGui::Button("Save to Device")) {
            device.SendConfig();
        }


        if (ImGui::Button("Select Output Folder") && !device.m_isCapturing) {
            auto ret = _OpenFolder();
            if (ret.has_value()) {
                device.m_outputPath = ret.value();
                device.UpdatePaths();
            }
        }

        if (device.m_isCapturing)
            ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::Text("%s", device.m_outputPath.c_str());

        if (device.m_outputPath != "") {
            if (!device.m_isCapturing && ImGui::Button("Start Capture")) {
                device.BeginCapture();
            } else if (device.m_isCapturing && ImGui::Button("Stop Capture")) {
                device.EndCapture();
            }
        }
    }

    float ret = ImGui::GetWindowSize().y;
    ImGui::End();

    return ret;
}

void ImGuiManager::RenderPlots(float y) {
    Device::Manager& device = Device::Manager::Get();
    const Device::FFTHandler& fft = device.GetFFT();
    
    if (!fft.HasSpeedFFT()) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - y));
    ImGui::SetNextWindowPos(ImVec2(0, y));

    ImGui::Begin("Plots", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar
    );


    float resolution = device.GetConfig().encoder_resolution;
    std::string plotName;
    int adcChannel = -1;

    if (ImGui::BeginTabBar("##plotBar")) {
        if (ImGui::BeginTabItem("Fourier")) {
            std::vector<double> mag;
            if(ImGui::BeginTabBar("##subplotbar")) {
                if(ImGui::BeginTabItem("Speed##fftbar")) {
                    plotName = "Speed FFT";
                    mag = fft.GetSpeedFFT();
                    ImGui::EndTabItem();
                }

                for(int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                    if(ImGui::BeginTabItem(std::format("ADC%d##fftbar", i+1).c_str())) {
                        plotName = std::format("ADC%d FFT", i);
                        mag = fft.GetADCFFT(i);
                        adcChannel = i;
                        ImGui::EndTabItem();
                    }

                }
                ImGui::EndTabBar();
            }

            if (adcChannel > -1) resolution /= (float)device.GetConfig().adc_div;

            std::vector<double> freq(mag.size(), 0);

            const double mult = resolution / (freq.size() * 2.0);
            for(size_t i = 0; i < freq.size(); i++) {
                freq[i] = i * mult;
            }

            if(ImPlot::BeginPlot(plotName.c_str(), ImVec2(-1, -1))) {
                ImPlot::SetupAxes("Order", "Magnitude");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 8.0);

                ImPlot::PlotScatter(plotName.c_str(), freq.data(), mag.data(), (int)mag.size());
                ImPlot::PlotLine(plotName.c_str(), freq.data(), mag.data(), (int)mag.size());

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Time Plot")) {
            std::vector<double> y1;
            if(ImGui::BeginTabBar("##subplotbar")) {
                if(ImGui::BeginTabItem("Speed")) {
                    plotName = "Speed";
                    y1 = fft.GetSpeedPlot();
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("ADC1")) {
                    plotName = "ADC1";
                    y1 = fft.GetADCPlot(0);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("ADC2")) {
                    plotName = "ADC2";
                    y1 = fft.GetADCPlot(1);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("ADC3")) {
                    plotName = "ADC3";
                    y1 = fft.GetADCPlot(2);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("ADC4")) {
                    plotName = "ADC4";
                    y1 = fft.GetADCPlot(3);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            

            std::vector<double> x1(y1.size(), 0);
            const double mult = resolution / (x1.size() * 2.0);
            for(size_t i = 0; i < x1.size(); i++) {
                x1[i] = i * mult;
            }

            if(ImPlot::BeginPlot(plotName.c_str(), ImVec2(-1, -1))) {
                ImPlot::SetupAxes("Time", "Value");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 8.0);

                ImPlot::PlotLine(plotName.c_str(), x1.data(), y1.data(), (int)y1.size());

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
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

    float y = this->RenderDeviceBar();
    this->RenderPlots(y);

    ImGui::End();
}



}
