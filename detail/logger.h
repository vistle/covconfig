// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file log.h
/// debug output
#pragma once

#include "export.h"
#include <ostream>
#include <string>

#ifdef CONFIG_NAMESPACE
#define CONFIG_STRINGIFY_(t) #t
#define CONFIG_STRINGIFY(t) CONFIG_STRINGIFY_(t)
#define CONFIG_NAME CONFIG_STRINGIFY(CONFIG_NAMESPACE)
#else
#define CONFIG_NAME ""
#endif

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {

/// debug output, switchable according to level
class COVEXPORT Logger {
public:
    Logger(const std::string &classname);
    virtual ~Logger();

protected:
    std::ostream &debug(const std::string &func = std::string()) const;
    std::ostream &info(const std::string &func = std::string()) const;
    std::ostream &warn(const std::string &func = std::string()) const;
    std::ostream &error(const std::string &func = std::string()) const;

private:
    std::string prefix(const std::string &func) const;
    std::ostream &getStream(int level) const;

    std::string m_name;
};

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
