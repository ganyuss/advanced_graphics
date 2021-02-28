cmake_minimum_required(VERSION 3.0)

project(RayTracer)

set (CMAKE_CXX_STANDARD 17)

set(SRCS main.cpp raytracer.cpp sphere.cpp light.cpp material.cpp triple.cpp lodepng.cpp scene.cpp Cone.cpp commongeometry.cpp Plane.cpp Quaternion.cpp Triangle.cpp TriangleAggregate.cpp glm.cpp box.cpp object.cpp)
file(GLOB YAML_SRCS "yaml/*.cpp")

file(GLOB YAML_HEADERS "yaml/*.h")
file(GLOB HEADERS "*.h")

find_package(OpenMP)
IF(OPENMP_FOUND)
    message("Found OpenMP! ^_^")
    # add flags for OpenMP
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_SHARED_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
ELSE()
    message("Missed OpenMP! x_x")
ENDIF()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_executable(hello ${SRCS} ${YAML_SRCS} ${HEADERS} ${YAML_HEADERS})
