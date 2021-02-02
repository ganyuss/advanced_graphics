cmake_minimum_required(VERSION 3.0)

project(RayTracer)

set (CMAKE_CXX_STANDARD 17)

set(SRCS main.cpp raytracer.cpp sphere.cpp light.cpp material.cpp image.cpp triple.cpp lodepng.cpp scene.cpp Cone.cpp commongeometry.cpp Plane.cpp Quaternion.cpp Triangle.cpp TriangleAggregate.cpp glm.cpp)
file(GLOB YAML_SRCS "yaml/*.cpp")

file(GLOB YAML_HEADERS "yaml/*.h")
file(GLOB HEADERS "*.h")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
add_executable(hello ${SRCS} ${YAML_SRCS} ${HEADERS} ${YAML_HEADERS})
