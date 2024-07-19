#include <GLFW/glfw3.h>
#include <cmath>
#include <stdio.h>
#include <memory>

#include "imgui_wrapper.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "state.hpp"
#include "gl_wrapper.hpp"
#include "world_gens/world_generators.hpp"

namespace imguiwrapper {

    // Internal variables
    static std::vector<std::string> worldGeneratorStrOptions;

    static int selectedWordGen = 0;
    static int log2n;
    static int log2maxn;

    static void setStyleGruvbox();

    void Initialize () {
        log2n = std::round(std::log2(*state::simulation::NPtr));
        log2maxn = std::round(std::log2(state::simulation::MAX_N));
        *state::simulation::NPtr = pow(2, log2n);

        worldGeneratorStrOptions.push_back("GALAXY");
        worldGeneratorStrOptions.push_back("TWO_GALAXIES");
        worldGeneratorStrOptions.push_back("SPHERE");
        worldGeneratorStrOptions.push_back("SPHERE_SHELL");
        worldGeneratorStrOptions.push_back("TWO_SPHERES");

        // Move initial option to the top
        for (int i = 0; i < worldGeneratorStrOptions.size(); i++) {
            if (state::simulation::WorldGeneratorPtr->ToString() == worldGeneratorStrOptions[i]) {
                std::string temp = worldGeneratorStrOptions[i];
                for (int j = i; j > 0; j--) {
                    worldGeneratorStrOptions[j] = worldGeneratorStrOptions[j - 1];
                }
                worldGeneratorStrOptions[0] = temp;
                break;
            }
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        setStyleGruvbox();
        ImGui_ImplGlfw_InitForOpenGL(glwrapper::Window, true);
        ImGui_ImplOpenGL3_Init("#version 150");
    }

    void Display () {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("Controls", NULL); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Bodies: %d", *state::simulation::NPtr);
        ImGui::SliderInt("log2(n)", &log2n, 0, log2maxn);
        *state::simulation::NPtr = pow(2, log2n);

        if (ImGui::BeginCombo("World Gen", worldGeneratorStrOptions[selectedWordGen].c_str())) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < worldGeneratorStrOptions.size(); n++)
            {
                const bool is_selected = (selectedWordGen == n);
                if (ImGui::Selectable(worldGeneratorStrOptions[n].c_str(), is_selected)) {
                    selectedWordGen = n;
                    std::string selectedWordGenStr = worldGeneratorStrOptions[selectedWordGen];
                    state::simulation::WorldGeneratorPtr = worldgens::FromString(selectedWordGenStr);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Spacing();
        ImGui::SliderFloat("Time step (y)", state::simulation::TimeStepPtr.get(), 0.0f, 10.0f);
        ImGui::SliderFloat("Epsilon (m)", state::simulation::EpsilonPtr.get(), 0.001f, 1.0f);

        ImGui::Spacing();
        ImGui::SliderFloat("Point size", state::rendering::PointSizePtr.get(), 0.0f, 5.0f);
        ImGui::SliderFloat("Camera speed", state::rendering::MainCameraSpeedPtr.get(), 1.0f, 5.0f);
        ImGui::SliderFloat("Mouse speed", state::rendering::MouseSensitivityPtr.get(), 0.0f, 0.5f);

        ImGui::Spacing();
        ImGui::Text("Sim time: %.2f My", *state::simulation::ElapsedTimePtr);
        ImGui::Text("Frametime: %.2f ms", glwrapper::DeltaTime * 1000);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Cleanup() {
        worldGeneratorStrOptions.clear();
    }

    void setStyleGruvbox () {
        auto& style = ImGui::GetStyle();
        style.ChildRounding = 0;
        style.GrabRounding = 0;
        style.FrameRounding = 0;
        style.PopupRounding = 0;
        style.ScrollbarRounding = 0;
        style.TabRounding = 0;
        style.WindowRounding = 0;
        style.FramePadding = { 4, 4 };

        style.WindowTitleAlign = { 0.5, 0.5 };

        ImVec4* colors = ImGui::GetStyle().Colors;
        // Updated to use IM_COL32 for more precise colors and to add table colors (1.80 feature)
        colors[ImGuiCol_Text] = ImColor { IM_COL32(0xeb, 0xdb, 0xb2, 0xFF) };
        colors[ImGuiCol_TextDisabled] = ImColor { IM_COL32(0x92, 0x83, 0x74, 0xFF) };
        colors[ImGuiCol_WindowBg] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0xF0) };
        colors[ImGuiCol_ChildBg] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0xFF) };
        colors[ImGuiCol_PopupBg] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0xF0) };
        colors[ImGuiCol_Border] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0xFF) };
        colors[ImGuiCol_BorderShadow] = ImColor { 0 };
        colors[ImGuiCol_FrameBg] = ImColor { IM_COL32(0x3c, 0x38, 0x36, 0x90) };
        colors[ImGuiCol_FrameBgHovered] = ImColor { IM_COL32(0x50, 0x49, 0x45, 0xFF) };
        colors[ImGuiCol_FrameBgActive] = ImColor { IM_COL32(0x66, 0x5c, 0x54, 0xA8) };
        colors[ImGuiCol_TitleBg] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0xFF) };
        colors[ImGuiCol_TitleBgActive] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xFF) };
        colors[ImGuiCol_TitleBgCollapsed] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0x9C) };
        colors[ImGuiCol_MenuBarBg] = ImColor { IM_COL32(0x28, 0x28, 0x28, 0xF0) };
        colors[ImGuiCol_ScrollbarBg] = ImColor { IM_COL32(0x00, 0x00, 0x00, 0x28) };
        colors[ImGuiCol_ScrollbarGrab] = ImColor { IM_COL32(0x3c, 0x38, 0x36, 0xFF) };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImColor { IM_COL32(0x50, 0x49, 0x45, 0xFF) };
        colors[ImGuiCol_ScrollbarGrabActive] = ImColor { IM_COL32(0x66, 0x5c, 0x54, 0xFF) };
        colors[ImGuiCol_CheckMark] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0x9E) };
        colors[ImGuiCol_SliderGrab] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0x70) };
        colors[ImGuiCol_SliderGrabActive] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xFF) };
        colors[ImGuiCol_Button] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0x66) };
        colors[ImGuiCol_ButtonHovered] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0x9E) };
        colors[ImGuiCol_ButtonActive] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xFF) };
        colors[ImGuiCol_Header] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0.4F) };
        colors[ImGuiCol_HeaderHovered] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xCC) };
        colors[ImGuiCol_HeaderActive] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xFF) };
        colors[ImGuiCol_Separator] = ImColor { IM_COL32(0x66, 0x5c, 0x54, 0.50f) };
        colors[ImGuiCol_SeparatorHovered] = ImColor { IM_COL32(0x50, 0x49, 0x45, 0.78f) };
        colors[ImGuiCol_SeparatorActive] = ImColor { IM_COL32(0x66, 0x5c, 0x54, 0xFF) };
        colors[ImGuiCol_ResizeGrip] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0x40) };
        colors[ImGuiCol_ResizeGripHovered] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xAA) };
        colors[ImGuiCol_ResizeGripActive] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xF2) };
        colors[ImGuiCol_Tab] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0xD8) };
        colors[ImGuiCol_TabHovered] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xCC) };
        colors[ImGuiCol_TabActive] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xFF) };
        colors[ImGuiCol_TabUnfocused] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0.97f) };
        colors[ImGuiCol_TabUnfocusedActive] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0xFF) };
        colors[ImGuiCol_PlotLines] = ImColor { IM_COL32(0xd6, 0x5d, 0x0e, 0xFF) };
        colors[ImGuiCol_PlotLinesHovered] = ImColor { IM_COL32(0xfe, 0x80, 0x19, 0xFF) };
        colors[ImGuiCol_PlotHistogram] = ImColor { IM_COL32(0x98, 0x97, 0x1a, 0xFF) };
        colors[ImGuiCol_PlotHistogramHovered] = ImColor { IM_COL32(0xb8, 0xbb, 0x26, 0xFF) };
        colors[ImGuiCol_TextSelectedBg] = ImColor { IM_COL32(0x45, 0x85, 0x88, 0x59) };
        colors[ImGuiCol_DragDropTarget] = ImColor { IM_COL32(0x98, 0x97, 0x1a, 0.90f) };
        colors[ImGuiCol_TableHeaderBg] = ImColor { IM_COL32(0x38, 0x3c, 0x36, 0xFF) };
        colors[ImGuiCol_TableBorderStrong] = ImColor { IM_COL32(0x28, 0x28, 0x28, 0xFF) };
        colors[ImGuiCol_TableBorderLight] = ImColor { IM_COL32(0x38, 0x3c, 0x36, 0xFF) };
        colors[ImGuiCol_TableRowBg] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0xFF) };
        colors[ImGuiCol_TableRowBgAlt] = ImColor { IM_COL32(0x28, 0x28, 0x28, 0xFF) };
        colors[ImGuiCol_TextSelectedBg] = ImColor { IM_COL32(0x45, 0x85, 0x88, 0xF0) };
        colors[ImGuiCol_NavHighlight] = ImColor { IM_COL32(0x83, 0xa5, 0x98, 0xFF) };
        colors[ImGuiCol_NavWindowingHighlight] = ImColor { IM_COL32(0xfb, 0xf1, 0xc7, 0xB2) };
        colors[ImGuiCol_NavWindowingDimBg] = ImColor { IM_COL32(0x7c, 0x6f, 0x64, 0x33) };
        colors[ImGuiCol_ModalWindowDimBg] = ImColor { IM_COL32(0x1d, 0x20, 0x21, 0x59) };
    }

}