set(COVCONFIG_SOURCES ${PREFIX}access.cpp ${PREFIX}array.cpp ${PREFIX}file.cpp ${PREFIX}value.cpp ${PREFIX}detail/base.cpp ${PREFIX}detail/entry.cpp ${PREFIX}detail/logger.cpp ${PREFIX}detail/manager.cpp ${PREFIX}detail/observer.cpp ${PREFIX}detail/output.cpp)

set(COVCONFIG_HEADERS ${PREFIX}access.h ${PREFIX}array.h ${PREFIX}file.h ${PREFIX}value.h ${PREFIX}config.h ${PREFIX}detail/output.h)
set(COVCONFIG_DETAIL_HEADERS ${PREFIX}detail/base.h ${PREFIX}detail/entry.h ${PREFIX}detail/export.h ${PREFIX}detail/flags.h ${PREFIX}detail/logger.h ${PREFIX}detail/manager.h ${PREFIX}detail/manager_impl.h ${PREFIX}detail/observer.h ${PREFIX}detail/output.h)

set(COVCONFIG_PRIVATE_INCLUDES ${PREFIX}detail/toml/include)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")
find_package(Filesystem)
if (Filesystem_FOUND)
    set(COVCONFIG_PRIVATE_LIBRARIES PRIVATE std::filesystem)
endif()
