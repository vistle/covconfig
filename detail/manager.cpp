// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "../value.h"
#include "../array.h"
#include "../access.h"
#include "manager.h"
#include "entry.h"

#include "manager_impl.h"

#include <toml++/toml.h>


#include <iostream>
#include <cstdio>

#ifdef _WIN32
#else
#include <unistd.h>
#endif

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
namespace detail {

static Manager *instance = nullptr;

static const std::string sep("/");

Manager *Manager::the()
{
#if 0
    if (!instance) {
        new Manager;
    }
#endif
    return instance;
}

bool Manager::exists()
{
    return instance != nullptr;
}


Manager::Manager(const std::string &host, const std::string &cluster, int rank)
: Logger("Manager"), m_hostname(host), m_cluster(cluster), m_rank(rank)
{
    if (instance) {
        debug() << "host=" << host << ", cluster=" << cluster << ", rank=" << rank
                << ", not overwriting existing instance" << std::endl;
    } else {
        instance = this;
        debug() << "host=" << host << ", cluster=" << cluster << ", rank=" << rank << ", NEW INSTANCE" << std::endl;
    }
    reconfigure();
}

bool Manager::setWorkspaceBridge(Bridge *bridge)
{
    if (m_bridge)
        return false;
    m_bridge = bridge;
    return true;
}

bool Manager::removeWorkspaceBridge(Bridge *bridge)
{
    if (m_bridge != bridge)
        return false;
    m_bridge = nullptr;
    return true;
}


void Manager::reconfigure()
{
#ifdef CONFIG_NAMESPACE
    const std::string cfg = CONFIG_NAME;
#else
    const std::string cfg = "covconfig";
#endif

#ifdef _WIN32
#else
    // current directory
    std::vector<char> cwd(PATH_MAX);
    if (const char *wd = getcwd(cwd.data(), cwd.size())) {
        m_path.push_back(wd + sep + "." + cfg);
    } else {
        error() << "cannot obtain current directory: " << strerror(errno) << std::endl;
    }
    // writable user config directory
    if (const char *xdg_ch = getenv("XDG_CONFIG_HOME")) {
        m_userPath = xdg_ch + sep + cfg;
    } else if (const char *home = getenv("HOME")) {
        m_userPath = home + sep + ".config" + sep + cfg;
    } else {
        error() << "cannot obtain HOME environment variable" << std::endl;
    }
    if (!m_userPath.empty()) {
        m_path.push_back(m_userPath);
    }
    // software installation directory
    if (!m_installPrefix.empty()) {
        m_path.push_back(m_installPrefix + sep + "config");
    }
    // global system configuration
    std::string dirs("/etc/xdg");
    if (const char *xdg_cds = getenv("XDG_CONFIG_DIRS")) {
        dirs = xdg_cds;
    }

    auto begin = dirs.begin();
    while (begin != dirs.end()) {
        auto end = std::find(begin, dirs.end(), ':');
        std::string dir(begin, end);
        if (!dir.empty()) {
            m_path.push_back(dir + sep + cfg);
        }
        begin = end;
        if (begin != dirs.end()) {
            begin = begin + 1;
        }
    }
#endif
}

void Manager::acquire()
{
    assert(m_useCount >= 0);
    ++m_useCount;
}

bool Manager::release()
{
    assert(m_useCount > 0);
    --m_useCount;

    if (m_useCount > 0)
        return false;

    delete this;
    return true;
}

Manager::~Manager()
{
    if (this == instance) {
        debug() << "destroying DEFAULT INSTANCE" << std::endl;
        instance = nullptr;
    } else {
        debug() << "destroying" << std::endl;
    }

#if 0
    info() << "saving config" << std::endl;
    for (auto &c: m_configs) {
        save(c.first);
    }
#endif

    for (auto &e: m_entries) {
        delete e.second;
    }
    m_entries.clear();
}

void Manager::setPrefix(const std::string &dir)
{
    m_installPrefix = dir;
    debug("setPrefix") << "installation prefix set to " << m_installPrefix << std::endl;
    reconfigure();
}

int Manager::rank() const
{
    return m_rank;
}

const std::string &Manager::cluster() const
{
    return m_cluster;
}

const std::string &Manager::hostname() const
{
    return m_hostname;
}

Config &Manager::registerEntry(const Entry *entry, const std::string &path, const std::string &section)
{
    auto it = m_configs.find(path);
    if (it != m_configs.end()) {
        return it->second;
    }

    std::vector<std::string> infixes;
    if (!m_cluster.empty() && !m_hostname.empty()) {
        infixes.push_back(sep + "c_" + m_cluster + sep + "h_" + m_hostname);
    }
    if (!m_hostname.empty()) {
        infixes.push_back(sep + "h_" + m_hostname);
    }
    if (!m_cluster.empty()) {
        infixes.push_back(sep + "c_" + m_cluster);
    }
    infixes.push_back("");

    for (const auto &infix: infixes) {
        for (const auto &basedir: m_path) {
            std::string dir = basedir + infix;
            std::string pathname = dir + sep + path + ".toml";
            toml::table tbl;
            try {
                tbl = toml::parse_file(pathname);
                debug("registerEntry") << pathname << " OK" << std::endl;
            } catch (toml::parse_error &ex) {
                if (ex.what() == std::string("File could not be opened for reading")) {
                    debug("registerEntry") << "could not parse " << pathname << ": " << ex.what() << std::endl;
                    continue;
                } else {
                    error("registerEntry") << "could not parse " << pathname << ": " << ex.what() << std::endl;
                    abort();
                    break;
                }
            } catch (std::exception &ex) {
                info("registerEntry") << "unhandled exception while parsing " << pathname << ": " << ex.what()
                                      << std::endl;
                continue;
            }
            Config config{path, tbl, dir, false};
            it = m_configs.emplace(path, config).first;
            return it->second;
        }
    }

    toml::table tbl;
    Config config{path, tbl, m_userPath, false};
    it = m_configs.emplace(path, config).first;
    return it->second;
}

bool Manager::sendToWorkspace(const ConfigBase *entry)
{
    if (!m_bridge)
        return false;
    return m_bridge->wasChanged(entry);
}

bool Manager::save(const std::string &path)
{
    auto it = m_configs.find(path);
    if (it == m_configs.end()) {
        return false;
    }

    if (m_userPath.empty()) {
        error("save") << "cannot save configuration " << path << ": no save path" << std::endl;
        return false;
    }

    std::string pathname = m_userPath + sep + path + ".toml";
    std::string temp = pathname + ".new";
    try {
        std::ofstream f(temp);
        f << it->second.config;
    } catch (std::exception &ex) {
        error("save") << "failed to save config to " << temp << ": " << ex.what() << std::endl;
        return false;
    }

    if (std::rename(temp.c_str(), pathname.c_str()) != 0) {
        error("save") << "failed to move updated config to " << pathname << std::endl;
        return false;
    }

    return true;
}

std::ostream &operator<<(std::ostream &os, const ConfigKey &key)
{
    os << key.path << ":" << key.section << ":" << key.name;
    return os;
}

template ValueEntry<bool> *Manager::getValue(const std::string &, const std::string &, const std::string &, Flag);
template ValueEntry<int64_t> *Manager::getValue(const std::string &, const std::string &, const std::string &, Flag);
template ValueEntry<double> *Manager::getValue(const std::string &, const std::string &, const std::string &, Flag);
template ValueEntry<std::string> *Manager::getValue(const std::string &, const std::string &, const std::string &,
                                                    Flag);
template ArrayEntry<bool> *Manager::getArray(const std::string &, const std::string &, const std::string &, Flag);
template ArrayEntry<int64_t> *Manager::getArray(const std::string &, const std::string &, const std::string &, Flag);
template ArrayEntry<double> *Manager::getArray(const std::string &, const std::string &, const std::string &, Flag);
template ArrayEntry<std::string> *Manager::getArray(const std::string &, const std::string &, const std::string &,
                                                    Flag);

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
