// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file file.h
/// query available settings within a file
#pragma once

#include <string>
#include <memory>
#include <vector>
#include "detail/export.h"
#include "detail/flags.h"
#include "detail/logger.h"
#include "section.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {
class Manager;
struct Config;
} // namespace detail
class ConfigBase;
template<class V>
class Value;
template<class V>
class Array;

/// query for existing sections and entries within a configuration file
class COVEXPORT File: public Section {
    friend class Section;
public:
    File(const std::string &path, detail::Manager *mgr = nullptr); ///< create an interface to a configuration file
    ~File(); ///< destructor

    bool exists() const; ///< query if path has existed when configuration was loaded
    std::string pathname() const; ///< actual path that was/would have been loaded
    bool save(); ///< request to store current configuration to disk
    void setSaveOnExit(
        bool enable); ///< request to save the current values when Manager is destroyed (i.e. application quits)
    bool isSaveOnExit() const; ///< query whether file will be saved automatically on exit
};

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
