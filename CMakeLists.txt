cmake_minimum_required(VERSION 3.22)
project(mqttproxy)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(mqttproxy main.cpp mqttproxy.cpp)

target_link_libraries(mqttproxy PUBLIC ${CONAN_LIBS})