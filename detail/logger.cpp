// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "logger.h"
#include "manager.h"
#include <iostream>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
namespace detail {
namespace {

enum Level {
    Error,
    Warn,
    Info,
    Debug,
    All,
};

class NullStream: public std::ostream {
public:
    NullStream(): std::ostream(&m_streambuf) {}

private:
    class NullBuffer: public std::streambuf {
    public:
        int overflow(int c) { return c; }
    };
    NullBuffer m_streambuf;
};

static NullStream null;

} // namespace


Logger::Logger(const std::string &classname): m_name(std::string(CONFIG_NAME) + "::config::" + classname)
{}

Logger::~Logger() = default;

std::string Logger::prefix(const std::string &func) const
{
    if (func.empty())
        return m_name + ": ";
    return m_name + "::" + func + ": ";
}

std::ostream &Logger::getStream(int level) const
{
    static int logLevel = -1;
    if (logLevel < 0) {
        if (const char *envLevel = getenv("COVCONFIG_DEBUG")) {
            if (envLevel[0] == '\0') {
                logLevel = All;
            } else if (std::string(CONFIG_NAME) == envLevel) {
                logLevel = All;
            } else {
                try {
                    logLevel = std::stoi(envLevel);
                } catch (...) {
                    logLevel = -1;
                }
                if (logLevel < 0)
                    logLevel = Info;
            }
        } else {
            logLevel = Info;
        }
    }
    if (level > logLevel)
        return null;
    return std::cerr;
}

std::ostream &Logger::debug(const std::string &func) const
{
    auto &str = getStream(Debug);
    str << "Debug: " << prefix(func);
    return str;
}

std::ostream &Logger::info(const std::string &func) const
{
    auto &str = getStream(Info);
    str << "Info: " << prefix(func);
    return str;
}

std::ostream &Logger::warn(const std::string &func) const
{
    auto &str = getStream(Warn);
    str << "Warn: " << prefix(func);
    return str;
}

std::ostream &Logger::error(const std::string &func) const
{
    auto &str = getStream(Error);
    str << "ERROR: " << prefix(func);
    return str;
}

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
