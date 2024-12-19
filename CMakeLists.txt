cmake_minimum_required(VERSION 3.21)
project(yarpscope-next )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(YARP REQUIRED)

# Add ImGui source files
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
set(IMPLOT_DIR "${CMAKE_SOURCE_DIR}/external/implot")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(IMPLOT_SOURCES
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
)

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/App.cpp
    src/DataVisualizer.cpp
    src/PlotWindow.cpp
    ${IMGUI_SOURCES}
    ${IMPLOT_SOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMPLOT_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${YARP_INCLUDE_DIRS}
    include/
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    ${OPENGL_LIBRARIES}
    ${YARP_LIBRARIES}
)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()