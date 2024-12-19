#include <PlotWindow.h>
#include <stdexcept>
#include <iostream>
#include <imgui.h>
#include <implot.h>

void LoadCustomFont(const char* fontPath, float fontSize) {
    ImGuiIO& io = ImGui::GetIO();
    
    // Load and add the font
    ImFont* customFont = io.Fonts->AddFontFromFileTTF(fontPath, fontSize);
    
    // Error checking
    if (customFont == nullptr) {
        std::cerr << "Failed to load font: " << fontPath << std::endl;
        return;
    }
    
    // Rebuild font atlas
    io.Fonts->Build();
}

PlotWindow::PlotWindow(int width, int height, const char* title) 
    : window(nullptr), display_w(width), display_h(height), 
      glsl_version("#version 130"), initialized(false) {
    
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    // GL 3.3 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
}

PlotWindow::~PlotWindow() {
    cleanup();
}

bool PlotWindow::init() {
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    setupImGui();
    initialized = true;
    return true;
}

void PlotWindow::setupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    glfwGetFramebufferSize(window, &display_w, &display_h);
    ImGui::GetIO().DisplaySize = ImVec2((float)display_w, (float)display_h);
    
    LoadCustomFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 36);
}

void PlotWindow::processEvents() {
    glfwPollEvents();
    glfwGetFramebufferSize(window, &display_w, &display_h);
    ImGui::GetIO().DisplaySize = ImVec2((float)display_w, (float)display_h);
}

void PlotWindow::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Rendering will be done by DataVisualizer
    
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

bool PlotWindow::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void PlotWindow::cleanup() {
    if (initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImPlot::DestroyContext();
    }
    
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
