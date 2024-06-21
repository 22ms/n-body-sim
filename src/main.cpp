// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#define GLEW_BUILD
#define GLFW_DLL

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLEW/glew.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // GRUVBOX
    auto &style = ImGui::GetStyle();
    style.ChildRounding = 0;
    style.GrabRounding = 0;
    style.FrameRounding = 0;
    style.PopupRounding = 0;
    style.ScrollbarRounding = 0;
    style.TabRounding = 0;
    style.WindowRounding = 0;
    style.FramePadding = {4, 4};

    style.WindowTitleAlign = {0.5, 0.5};

    ImVec4 *colors = ImGui::GetStyle().Colors;
    // Updated to use IM_COL32 for more precise colors and to add table colors (1.80 feature)
    colors[ImGuiCol_Text] = ImColor{IM_COL32(0xeb, 0xdb, 0xb2, 0xFF)};
    colors[ImGuiCol_TextDisabled] = ImColor{IM_COL32(0x92, 0x83, 0x74, 0xFF)};
    colors[ImGuiCol_WindowBg] = ImColor{IM_COL32(0x1d, 0x20, 0x21, 0xF0)};
    colors[ImGuiCol_ChildBg] = ImColor{IM_COL32(0x1d, 0x20, 0x21, 0xFF)};
    colors[ImGuiCol_PopupBg] = ImColor{IM_COL32(0x1d, 0x20, 0x21, 0xF0)};
    colors[ImGuiCol_Border] = ImColor{IM_COL32(0x1d, 0x20, 0x21, 0xFF)};
    colors[ImGuiCol_BorderShadow] = ImColor{0};
    colors[ImGuiCol_FrameBg] = ImColor{IM_COL32(0x3c, 0x38, 0x36, 0x90)};
    colors[ImGuiCol_FrameBgHovered] = ImColor{IM_COL32(0x50, 0x49, 0x45, 0xFF)};
    colors[ImGuiCol_FrameBgActive] = ImColor{IM_COL32(0x66, 0x5c, 0x54, 0xA8)};
    colors[ImGuiCol_TitleBg] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0xFF)};
    colors[ImGuiCol_TitleBgActive] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xFF)};
    colors[ImGuiCol_TitleBgCollapsed] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0x9C)};
    colors[ImGuiCol_MenuBarBg] = ImColor{IM_COL32(0x28, 0x28, 0x28, 0xF0)};
    colors[ImGuiCol_ScrollbarBg] = ImColor{IM_COL32(0x00, 0x00, 0x00, 0x28)};
    colors[ImGuiCol_ScrollbarGrab] = ImColor{IM_COL32(0x3c, 0x38, 0x36, 0xFF)};
    colors[ImGuiCol_ScrollbarGrabHovered] = ImColor{IM_COL32(0x50, 0x49, 0x45, 0xFF)};
    colors[ImGuiCol_ScrollbarGrabActive] = ImColor{IM_COL32(0x66, 0x5c, 0x54, 0xFF)};
    colors[ImGuiCol_CheckMark] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0x9E)};
    colors[ImGuiCol_SliderGrab] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0x70)};
    colors[ImGuiCol_SliderGrabActive] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xFF)};
    colors[ImGuiCol_Button] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0x66)};
    colors[ImGuiCol_ButtonHovered] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0x9E)};
    colors[ImGuiCol_ButtonActive] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xFF)};
    colors[ImGuiCol_Header] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0.4F)};
    colors[ImGuiCol_HeaderHovered] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xCC)};
    colors[ImGuiCol_HeaderActive] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xFF)};
    colors[ImGuiCol_Separator] = ImColor{IM_COL32(0x66, 0x5c, 0x54, 0.50f)};
    colors[ImGuiCol_SeparatorHovered] = ImColor{IM_COL32(0x50, 0x49, 0x45, 0.78f)};
    colors[ImGuiCol_SeparatorActive] = ImColor{IM_COL32(0x66, 0x5c, 0x54, 0xFF)};
    colors[ImGuiCol_ResizeGrip] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0x40)};
    colors[ImGuiCol_ResizeGripHovered] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xAA)};
    colors[ImGuiCol_ResizeGripActive] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xF2)};
    colors[ImGuiCol_Tab] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0xD8)};
    colors[ImGuiCol_TabHovered] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xCC)};
    colors[ImGuiCol_TabActive] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xFF)};
    colors[ImGuiCol_TabUnfocused] = ImColor{IM_COL32(0x1d, 0x20, 0x21, 0.97f)};
    colors[ImGuiCol_TabUnfocusedActive] = ImColor{IM_COL32(0x1d, 0x20, 0x21, 0xFF)};
    colors[ImGuiCol_PlotLines] = ImColor{IM_COL32(0xd6, 0x5d, 0x0e, 0xFF)};
    colors[ImGuiCol_PlotLinesHovered] = ImColor{IM_COL32(0xfe, 0x80, 0x19, 0xFF)};
    colors[ImGuiCol_PlotHistogram] = ImColor{IM_COL32(0x98, 0x97, 0x1a, 0xFF)};
    colors[ImGuiCol_PlotHistogramHovered] = ImColor{IM_COL32(0xb8, 0xbb, 0x26, 0xFF)};
    colors[ImGuiCol_TextSelectedBg] = ImColor{IM_COL32(0x45, 0x85, 0x88, 0x59)};
    colors[ImGuiCol_DragDropTarget] = ImColor{IM_COL32(0x98, 0x97, 0x1a, 0.90f)};
    colors[ImGuiCol_TableHeaderBg] = ImColor{IM_COL32(0x38, 0x3c, 0x36, 0xFF)};
    colors[ImGuiCol_TableBorderStrong] = ImColor{IM_COL32(0x28, 0x28, 0x28, 0xFF)};
    colors[ImGuiCol_TableBorderLight] = ImColor{IM_COL32(0x38, 0x3c, 0x36, 0xFF)};
    colors[ImGuiCol_TableRowBg] = ImColor {IM_COL32(0x1d, 0x20, 0x21, 0xFF)};
    colors[ImGuiCol_TableRowBgAlt] = ImColor{IM_COL32(0x28, 0x28, 0x28, 0xFF)};
    colors[ImGuiCol_TextSelectedBg] = ImColor { IM_COL32(0x45, 0x85, 0x88, 0xF0) };
    colors[ImGuiCol_NavHighlight] = ImColor{IM_COL32(0x83, 0xa5, 0x98, 0xFF)};
    colors[ImGuiCol_NavWindowingHighlight] = ImColor{IM_COL32(0xfb, 0xf1, 0xc7, 0xB2)};
    colors[ImGuiCol_NavWindowingDimBg] = ImColor{IM_COL32(0x7c, 0x6f, 0x64, 0x33)};
    colors[ImGuiCol_ModalWindowDimBg] = ImColor{IM_COL32(0x1d, 0x20, 0x21, 0x59)};
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}