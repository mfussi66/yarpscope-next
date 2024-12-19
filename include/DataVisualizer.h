#pragma once
#include <vector>

class DataVisualizer {
public:
    DataVisualizer();
    
    void render(const std::vector<float>& data, const std::vector<float>& timestamps);
    void updatePlotLimits();
    
private:
    float plotMinY;
    float plotMaxY;
    float markerSize;
    float lineWeight;
    
    void renderPlotControls();
    void renderPlot(const std::vector<float>& data, const std::vector<float>& timestamps);
};