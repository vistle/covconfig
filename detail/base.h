// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file base.h
/// common base class for all \ref Array's and \ref Value's
#pragma once

#include "export.h"
#include "logger.h"
#include "observer.h"
#include <string>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {
class Entry;
}

/// polymorphic access to \ref Value's and \ref Array's
class COVEXPORT ConfigBase: protected detail::Logger, protected detail::Observer {
public:
    using Logger::Logger;
    virtual ~ConfigBase();

    bool exists() const; ///< query whether data for entry was found in configuration file
    const std::string &path() const; ///< path fragment to configuration file, appended to various search prefixes
    const std::string &section() const; ///< section within configuration file
    const std::string &name() const; ///< name of entry within section in configuration file
    std::string key() const; ///< path, section and name combined into a single string, convenience for debug output

protected:
    detail::Entry *m_entry = nullptr; ///< access to runtime storage of data
};
}
#ifdef CONFIG_NAMESPACE
}
#endif
