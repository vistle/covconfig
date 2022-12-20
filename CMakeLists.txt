cmake_minimum_required(VERSION 3.15.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
    Config
    DESCRIPTION "Config wrapper for COVISE, OpenCOVER and Vistle"
    LANGUAGES CXX)

set(CONFIG_SOURCES
    array.h array.cpp config.cpp config.h entry.cpp entry.h entry_impl.h manager.h manager.cpp manager_impl.h value.h value.cpp observer.h observer.cpp)

if(VISTLE_LIB_TYPE)
    set(TARGET vistle_config)
    vistle_add_library(${TARGET} EXPORT ${VISTLE_LIB_TYPE} ${CONFIG_SOURCES})
    target_compile_definitions(${TARGET} PUBLIC CONFIG_NAMESPACE=vistle)
    target_include_directories(${TARGET} PRIVATE ../..)
else()
    set(TARGET config)
    add_library(${TARGET} ${CONFIG_SOURCES})
endif()
target_include_directories(${TARGET} PRIVATE toml/include)
