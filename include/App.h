#pragma once
#include "PlotWindow.h"
#include "DataVisualizer.h"
#include "YarpReader.h"
#include <memory>

class App {
public:
    App();
    ~App();
    
    bool init();
    void run();
    
private:
    std::unique_ptr<PlotWindow> window;
    std::unique_ptr<DataVisualizer> visualizer;
    std::unique_ptr<YarpDataReader> dataReader;
};
