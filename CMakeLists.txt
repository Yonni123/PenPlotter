cmake_minimum_required(VERSION 3.12)
project(YourProjectName)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find Potrace
find_library(POTRACE_LIBRARY NAMES potrace)
include_directories("/usr/include")

# Add the executable
add_executable(main main.cpp)

# Link against OpenCV and Potrace
target_link_libraries(main ${OpenCV_LIBS} ${POTRACE_LIBRARY})
