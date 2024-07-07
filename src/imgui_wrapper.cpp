#include <GLFW/glfw3.h>

#include "imgui_wrapper.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void ImGuiWrapper::Initialize (GLFWwindow* window, int* N, float* cameraSpeed, float* timeScale) {
    m_N = N;
    m_CameraSpeed = cameraSpeed;
    m_TimeScale = timeScale;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    setStyleGruvbox();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void ImGuiWrapper::Display () {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::Begin("Controls", NULL); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::InputInt("Bodies", m_N);
    ImGui::SliderFloat("Time scale", m_TimeScale, 0.0f, 1.0f);
    ImGui::SliderFloat("Camera speed", m_CameraSpeed, 1.0f, 5.0f);
    ImGui::End();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiWrapper::setStyleGruvbox () {
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