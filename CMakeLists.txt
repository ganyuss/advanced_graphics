# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.0)

# Notre projet est étiqueté hello
project(hello)

# Crée des variables avec les fichiers à compiler
set(SRCS main.cpp raytracer.cpp sphere.cpp light.cpp material.cpp image.cpp triple.cpp lodepng.cpp scene.cpp)
file(GLOB YAML_SRCS "yaml/*.cpp")

file(GLOB YAML_HEADERS "yaml/*.h")
file(GLOB HEADERS "*.h")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
add_executable(hello ${SRCS} ${YAML_SRCS} ${HEADERS} ${YAML_HEADERS})