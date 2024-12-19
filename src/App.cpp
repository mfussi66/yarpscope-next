#include <App.h>

App::App()
    : window(std::make_unique<PlotWindow>(1280, 720, "YARP Data Plot")),
      visualizer(std::make_unique<DataVisualizer>()),
      dataReader(std::make_unique<YarpDataReader>()) {
}

App::~App() = default;

bool App::init() {
    return window->init();
}

void App::run() {
    while (!window->shouldClose()) {
        window->processEvents();
        
        std::vector<float> plotData, plotTimestamps;
        dataReader->getData(plotData, plotTimestamps);
        
        visualizer->render(plotData, plotTimestamps);
        window->render();
    }
}
