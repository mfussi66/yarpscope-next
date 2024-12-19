#pragma once

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <memory>
#include <vector>

class PlotWindow {
public:
    PlotWindow(int width, int height, const char* title);
    ~PlotWindow();
    
    bool init();
    bool shouldClose() const;
    void processEvents();
    void render();

private:
    GLFWwindow* window;
    int display_w;
    int display_h;
    const char* glsl_version;
    bool initialized;
    
    void setupImGui();
    void cleanup();
};