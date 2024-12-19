#include <imgui.h>
#include <implot.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <deque>
#include <thread>
#include <chrono>
#include <iostream>
#include <yarp/os/all.h>

class YarpDataReader {
private:
    std::deque<float> data;
    std::deque<float> timestamps;
    const size_t maxDataPoints = 100000;
    std::mutex dataMutex;
    bool running;
    yarp::os::BufferedPort<yarp::os::Bottle> port;
    std::thread readThread;
    double t0;

public:
    YarpDataReader() : running(true) {
        // Initialize YARP network
        static yarp::os::Network yarp;
        if (!yarp.checkNetwork()) {
            throw std::runtime_error("YARP network not available");
        }

        // Open the port
        if (!port.open("/plot/input")) {
            throw std::runtime_error("Failed to open port");
        }

        // Start the reading thread
        readThread = std::thread([this]() { readLoop(); });

        t0  = yarp::os::Time::now();
    }

    ~YarpDataReader() {
        running = false;
        if (readThread.joinable()) {
            readThread.join();
        }
        port.close();
    }

    void readLoop() {
        while (running) {
            yarp::os::Bottle* bottle = port.read(false);
            if (bottle != nullptr) {
                std::lock_guard<std::mutex> lock(dataMutex);
                
                // Read the first value from the bottle
                float value = bottle->get(0).asFloat32();
                float timestamp = yarp::os::Time::now() - t0;
                
                data.push_back(value);
                timestamps.push_back(timestamp);
                
                while (data.size() > maxDataPoints) {
                    data.pop_front();
                    timestamps.pop_front();
                }
            }
            // Small sleep to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void getData(std::vector<float>& outData, std::vector<float>& outTimestamps) {
        std::lock_guard<std::mutex> lock(dataMutex);
        outData = std::vector<float>(data.begin(), data.end());
        outTimestamps = std::vector<float>(timestamps.begin(), timestamps.end());
    }

    bool isRunning() const { return running; }
    void stop() { running = false; }
};


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

int main() {
    try {
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return -1;
        }

        // GL 3.3 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        // Create window with graphics context
        GLFWwindow* window = glfwCreateWindow(1280, 720, "YARP Data Plot", nullptr, nullptr);
        if (window == nullptr) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // Initialize Dear ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();

        // Configure ImGui style
        ImGui::StyleColorsDark();
        
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Set display size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui::GetIO().DisplaySize = ImVec2((float)display_w, (float)display_h);

        LoadCustomFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 36);

        // Create YARP data reader
        YarpDataReader reader;
        float plotMinY = -10.0f;  // Adjust these based on your data range
        float plotMaxY = 10.0f;

        // Main loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Update display size every frame
            glfwGetFramebufferSize(window, &display_w, &display_h);
            ImGui::GetIO().DisplaySize = ImVec2((float)display_w, (float)display_h);

            // Create a window that takes up the entire screen
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::Begin("YARP Data Plot", nullptr, 
                        ImGuiWindowFlags_NoTitleBar | 
                        ImGuiWindowFlags_NoResize | 
                        ImGuiWindowFlags_NoMove | 
                        ImGuiWindowFlags_NoCollapse);
            
            // Add Y-axis range controls
            ImGui::SliderFloat("Y Min", &plotMinY, -10000.0f, 0.0f);
            ImGui::SliderFloat("Y Max", &plotMaxY, 0.0f, 10000.0f);
            
            if (ImPlot::BeginPlot("##YARP Data", ImVec2(-1, -1))) {
                ImPlot::SetupAxes("Time (s)", "Value");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, 10, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, plotMinY, plotMaxY, ImGuiCond_Always);

                std::vector<float> plotData, plotTimestamps;
                reader.getData(plotData, plotTimestamps);

                if (!plotData.empty()) {
                    // Adjust timestamps to be relative to the latest time
                    if (!plotTimestamps.empty()) {
                        float latestTime = plotTimestamps.back();
                        for (auto& t : plotTimestamps) {
                            t -= (latestTime - 10.0f);
                        }
                    }
                    
                    // Set marker properties
                    //ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, 6.0f);  // Make markers bigger
                    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 6.0f);  // Make line thicker
                    
                    // Plot with both lines and markers
                    ImPlot::PlotLine("Data", plotTimestamps.data(), plotData.data(), 
                                   plotData.size(), ImPlotLineFlags_None);
                   /*  ImPlot::PlotScatter("##DataPoints", plotTimestamps.data(), plotData.data(), 
                                      plotData.size()); */
                    
                    // Add controls for marker and line size
                    static float markerSize = 6.0f;
                    static float lineWeight = 6.0f;
                    ImGui::SetNextItemWidth(200);
                    if (ImGui::SliderFloat("Marker Size", &markerSize, 2.0f, 20.0f)) {
                        ImPlot::GetStyle().MarkerSize = markerSize;
                    }
                    ImGui::SetNextItemWidth(200);
                    if (ImGui::SliderFloat("Line Weight", &lineWeight, 1.0f, 10.0f)) {
                        ImPlot::GetStyle().LineWeight = lineWeight;
                    }
                    
                    ImPlot::PopStyleVar(1);  // Pop both style variables
                }

                ImPlot::EndPlot();
            }

            ImGui::End();

            // Rendering
            ImGui::Render();
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        // Cleanup
        reader.stop();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImPlot::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}