cmake_minimum_required(VERSION 3.15.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
    CovConfig
    DESCRIPTION "Config wrapper for COVISE, OpenCOVER and Vistle"
    LANGUAGES CXX)

set(COVCONFIG_SOURCES access.cpp array.cpp value.cpp detail/base.cpp detail/entry.cpp detail/logger.cpp detail/manager.cpp detail/observer.cpp detail/output.cpp)
set(COVCONFIG_HEADERS access.h array.h value.h detail/base.h config.h detail/entry.h detail/export.h detail/flags.h detail/logger.h detail/manager.h detail/manager_impl.h detail/observer.h detail/output.h)

if(VISTLE_LIB_TYPE)
    set(TARGET vistle_covconfig)
    vistle_add_library(${TARGET} EXPORT ${VISTLE_LIB_TYPE} ${COVCONFIG_SOURCES} ${COVCONFIG_HEADERS})
    target_compile_definitions(${TARGET} PRIVATE CONFIG_NAMESPACE=vistle COVCONFIG_VISTLE)
elseif(COVISE_BUILDING_COVER)
    set(TARGET coOpenCovConfig)
    add_covise_library(${TARGET} ${COVCONFIG_SOURCES} ${COVCONFIG_HEADERS})
    target_compile_definitions(${TARGET} PRIVATE CONFIG_NAMESPACE=opencover COVCONFIG_OPENCOVER)
    covise_install_target(coOpenCovConfig)
    covise_install_headers(covconfig ${COVCONFIG_HEADERS})
elseif(COVISE_BUILDING_COVISE)
    set(TARGET coCovConfig)
    add_covise_library(${TARGET} ${COVCONFIG_SOURCES} ${COVCONFIG_HEADERS})
    target_compile_definitions(${TARGET} PRIVATE CONFIG_NAMESPACE=covise COVCONFIG_COVISE)
    covise_install_target(coCovConfig)
    covise_install_headers(covconfig ${COVCONFIG_HEADERS})
else()
    set(TARGET covconfig)
    add_library(${TARGET} ${COVCONFIG_SOURCES} ${COVCONFIG_HEADERS})
endif()
target_include_directories(${TARGET} PRIVATE detail/toml/include)
