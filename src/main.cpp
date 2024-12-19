#include <App.h>
#include <iostream>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

int main(int argc, char **argv) {
  yarp::os::Network yarp;

  if (!yarp::os::Network::checkNetwork()) {
    yError() << "Unable to find YARP server!";
    return EXIT_FAILURE;
  }

  yarp::os::ResourceFinder rf;
  rf.setVerbose(false);
  rf.setQuiet(true);

  rf.configure(argc, argv);

    App app;
    if (!app.init(rf)) {
        return -1;
    }
    app.run();
    return 0;
}