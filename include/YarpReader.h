#include <thread>
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
    std::string portPrefix = "/scope_";
    std::string remotePort = "/in";
    std::vector<uint16_t> idx = {0};

public:
    YarpDataReader(const std::string & remote, const yarp::os::Bottle & indices) : 
    running(true), 
    remotePort(remote) {
        // Initialize YARP network
        static yarp::os::Network yarp;
        if (!yarp.checkNetwork()) {
            throw std::runtime_error("YARP network not available");
        }

        uint8_t port_idx = 0;
        // Open the port
        if (!port.open(portPrefix + std::to_string(port_idx) + ":i")) {
            throw std::runtime_error("Failed to open port");
        }

        if(!yarp::os::Network::connect(remote, port.getName()))
        {
            throw std::runtime_error("Failed to connect to " + remote);
        }

        for (size_t i = 0; i < indices.size(); i++)
        {
            idx.push_back(indices.get(i).asInt16());
            std::cerr << std::to_string(indices.get(i).asInt16()) << std::endl;
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
                float value = bottle->get(idx[0]).asFloat32();
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