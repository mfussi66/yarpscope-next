#include <DataVisualizer.h>

#include <imgui.h>
#include <implot.h>

DataVisualizer::DataVisualizer()
    : plotMinY(-10.0f), plotMaxY(10.0f),
      markerSize(6.0f), lineWeight(6.0f) {
}

void DataVisualizer::render(const std::vector<float>& data, 
                          const std::vector<float>& timestamps) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("YARP Data Plot", nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse);
    
    renderPlotControls();
    renderPlot(data, timestamps);
    
    ImGui::End();
}

void DataVisualizer::renderPlotControls() {
    ImGui::SliderFloat("Y Min", &plotMinY, -10000.0f, 0.0f);
    ImGui::SliderFloat("Y Max", &plotMaxY, 0.0f, 10000.0f);
    
    ImGui::SetNextItemWidth(200);
    if (ImGui::SliderFloat("Marker Size", &markerSize, 2.0f, 20.0f)) {
        ImPlot::GetStyle().MarkerSize = markerSize;
    }
    
    ImGui::SetNextItemWidth(200);
    if (ImGui::SliderFloat("Line Weight", &lineWeight, 1.0f, 10.0f)) {
        ImPlot::GetStyle().LineWeight = lineWeight;
    }
}

void DataVisualizer::renderPlot(const std::vector<float>& data,
                              const std::vector<float>& timestamps) {
    if (ImPlot::BeginPlot("##YARP Data", ImVec2(-1, -1))) {
        ImPlot::SetupAxes("Time (s)", "Value");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 10, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, plotMinY, plotMaxY, ImGuiCond_Always);
        
        if (!data.empty() && !timestamps.empty()) {
            std::vector<float> adjustedTimestamps = timestamps;
            float latestTime = timestamps.back();
            for (auto& t : adjustedTimestamps) {
                t -= (latestTime - 10.0f);
            }
            
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, lineWeight);
            ImPlot::PlotLine("Data", adjustedTimestamps.data(), data.data(),
                            data.size(), ImPlotLineFlags_None);
            ImPlot::PopStyleVar(1);
        }
        
        ImPlot::EndPlot();
    }
}