#include <App.h>

App::App() {}

App::~App() = default;

bool App::init(const yarp::os::ResourceFinder &rf) {

  std::string port;
  port = rf.check("port", yarp::os::Value("/in")).asString();
  if(port.empty())
  {
    std::cerr << "Wrong port name: " + port << std::endl;
    return false;
  }

  yarp::os::Bottle indices({yarp::os::Value(0)});

  if(rf.check("index"))
  {
    indices.copy(*rf.find("index").asList());
  }

  dataReader = std::make_unique<YarpDataReader>(port, indices);
  window = std::make_unique<PlotWindow>(1280, 720, "YARP Data Plot");
  visualizer = std::make_unique<DataVisualizer>();
  return window->init();
  }

void App::run() {
  while (!window->shouldClose()) {
    window->processEvents();

    std::vector<float> plotData, plotTimestamps;
    dataReader->getData(plotData, plotTimestamps);

    window->render([&]() { visualizer->render(plotData, plotTimestamps); });
  }
}
