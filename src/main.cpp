#include <App.h>
#include <iostream>

int main() {
    try {
        App app;
        if (!app.init()) {
            return -1;
        }
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}