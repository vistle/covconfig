// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file manager.h
/// manage access to configuration data
#pragma once

#include <string>
#include <memory>
#include <map>

#include "entry.h"
#include "base.h"
#include "logger.h"

#include <toml++/toml.h>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

class Access;
class Bridge;

namespace detail {

struct Config {
    std::string path; // path fragment
    std::string base; // base directory
    toml::table config;
    bool exists = false;
    bool modified = false;
};

struct ConfigKey {
    std::string path;
    std::string section;
    std::string name;

    bool operator<(const ConfigKey &o) const
    {
        if (path == o.path) {
            if (section == o.section) {
                return name < o.name;
            }
            return section < o.section;
        }
        return path < o.path;
    }
};
std::ostream &operator<<(std::ostream &os, const ConfigKey &key);

class Manager: Logger {
    friend class config::Access;

public:
    static Manager *the();
    static bool exists();
    const std::string &hostname() const;
    const std::string &cluster() const;
    int rank() const;

    Config &registerPath(const std::string &path);
    template<class V>
    ValueEntry<V> *getValue(const std::string &path, const std::string &section, const std::string &name, Flag flags);
    template<class V>
    ArrayEntry<V> *getArray(const std::string &path, const std::string &section, const std::string &name, Flag flags);

    bool save(const std::string &path);

    bool sendToWorkspace(const ConfigBase *value);

private:
    Manager(const std::string &host, const std::string &cluster = std::string(), int rank = -1);
    ~Manager();
    bool setWorkspaceBridge(Bridge *bridge);
    bool removeWorkspaceBridge(Bridge *bridge);
    void setPrefix(const std::string &dir);
    void acquire();
    bool release();
    void reconfigure();

    std::string m_hostname;
    const std::string m_cluster;
    const int m_rank = -1;

    int m_useCount = 0;

    std::string m_userPath; // where to write configuration
    std::string m_installPrefix;
    std::vector<std::string> m_path;
    std::map<std::string, Config> m_configs;

    typedef ConfigKey Key;
    std::map<Key, Entry *> m_entries;
    Bridge *m_bridge = nullptr;
};

extern template ValueEntry<bool> *Manager::getValue(const std::string &, const std::string &, const std::string &,
                                                    Flag);
extern template ValueEntry<int64_t> *Manager::getValue(const std::string &, const std::string &, const std::string &,
                                                       Flag);
extern template ValueEntry<double> *Manager::getValue(const std::string &, const std::string &, const std::string &,
                                                      Flag);
extern template ValueEntry<std::string> *Manager::getValue(const std::string &, const std::string &,
                                                           const std::string &, Flag);
extern template ArrayEntry<bool> *Manager::getArray(const std::string &, const std::string &, const std::string &,
                                                    Flag);
extern template ArrayEntry<int64_t> *Manager::getArray(const std::string &, const std::string &, const std::string &,
                                                       Flag);
extern template ArrayEntry<double> *Manager::getArray(const std::string &, const std::string &, const std::string &,
                                                      Flag);
extern template ArrayEntry<std::string> *Manager::getArray(const std::string &, const std::string &,
                                                           const std::string &, Flag);

} // namespace detail
}
#ifdef CONFIG_NAMESPACE
}
#endif
