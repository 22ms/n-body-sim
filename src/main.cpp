// References:
// https://iss.oden.utexas.edu/Publications/Papers/burtscher11.pdf

#define GLEW_BUILD
#define GLFW_DLL

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLEW/glew.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "opencl_wrapper/opencl.hpp"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void initialize_opencl()
{
    Device device(select_device_with_most_flops()); // compile OpenCL C code for the fastest available device

    const uint N = 1024u; // size of vectors
    Memory<float> A(device, N); // allocate memory on both host and device
    Memory<float> B(device, N);
    Memory<float> C(device, N);

    Kernel add_kernel(device, N, "add_kernel", A, B, C); // kernel that runs on the device

    for (uint n = 0u; n < N; n++) {
        A[n] = 4.0f; // initialize memory
        B[n] = 2.0f;
        C[n] = 1.0f;
    }

    print_info("Value before kernel execution: C[0] = " + to_string(C[0]));

    A.write_to_device(); // copy data from host memory to device memory
    B.write_to_device();
    add_kernel.run(); // run add_kernel on the device
    C.read_from_device(); // copy data from device memory to host memory

    print_info("Value after kernel execution: C[0] = " + to_string(C[0]));
}

// Main code
int main(int, char**)
{
    initialize_opencl();

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    int width = 1280;
    int height = 720;
    GLFWwindow* window = glfwCreateWindow(width, height, "N-body simulation", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    /* #region Style */
    // GRUVBOX
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

    /* #endregion */

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    int N = 500;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("Controls", NULL); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::InputInt("N of bodies", &N);
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}