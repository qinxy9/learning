cmake_minimum_required(VERSION 3.16)
project(learning)



set(OpenCV_DIR "~/lib/opencv4.10.0_install/lib/cmake/opencv4")

find_package(OpenCV 4.10 REQUIRED)

set(CMAKE_CXX_STANDARD 17)


# find_package(OpenCV REQUIRED)
message("OpenCV version  is ： ${OpenCV_VERSION}")

add_executable(learning main.cpp)

target_link_libraries(learning PRIVATE ${OpenCV_LIBS})
