cmake_minimum_required(VERSION 3.15.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
    CovConfig
    DESCRIPTION "Config wrapper for COVISE, OpenCOVER and Vistle"
    LANGUAGES CXX)

include(covconfig.cmake)

add_library(covconfig ${COVCONFIG_SOURCES} ${COVCONFIG_HEADERS})
target_include_directories(covconfig PRIVATE ${COVCONFIG_PRIVATE_INCLUDES})
