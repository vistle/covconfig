#include "manager.h"
#include "entry.h"

#include <toml++/toml.h>

#include <iostream>
#include <cstdio>
#include "manager_impl.h"

#ifdef _WIN32
#else
#include <unistd.h>
#endif

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

static Manager *instance = nullptr;

static const std::string sep("/");

Manager *Manager::the()
{
    if (!instance) {
        new Manager;
    }
    return instance;
}

bool Manager::exists()
{
    return instance != nullptr;
}


Manager::Manager(const std::string &cluster, int rank): m_cluster(cluster), m_rank(rank)
{
    instance = this;
    reconfigure();
}

void Manager::reconfigure()
{
#ifdef CONFIG_NAMESPACE
#define STR(t) #t
#define S(t) STR(t)
    const std::string cfg = S(CONFIG_NAMESPACE);
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
        std::cerr << "config::Manager: cannot obtain current directory: " << strerror(errno) << std::endl;
    }
    // writable user config directory
    if (const char *xdg_ch = getenv("XDG_CONFIG_HOME")) {
        m_userPath = xdg_ch + sep + cfg;
    } else if (const char *home = getenv("HOME")) {
        m_userPath = home + sep + ".config" + sep + cfg;
    } else {
        std::cerr << "config::Manager: cannot obtain HOME environment variable" << std::endl;
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
    std::cerr << "config::Manager: destroying" << std::endl;
#if 0
    std::cerr << "config::Manager: saving config" << std::endl;
    for (auto &c: m_configs) {
        save(c.first);
    }
#endif
    instance = nullptr;
}

void Manager::setPrefix(const std::string &dir)
{
    m_installPrefix = dir;
    std::cerr << "config::Manager: installation prefix set to " << m_installPrefix << std::endl;
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

Config &Manager::registerEntry(const EntryBase *entry, const std::string &path, const std::string &section)
{
    auto it = m_configs.find(path);
    if (it != m_configs.end()) {
        return it->second;
    }

    for (const auto &basedir: m_path) {
        std::vector<std::string> dirs;
        if (!m_cluster.empty()) {
            dirs.push_back(basedir + sep + m_cluster);
        }
        dirs.push_back(basedir);
        for (auto &dir: dirs) {
            std::string pathname = dir + sep + path + ".toml";
            toml::table tbl;
            try {
                tbl = toml::parse_file(pathname);
            } catch (std::exception &ex) {
                std::cerr << "config::Manager: could not parse " << pathname << ": " << ex.what() << std::endl;
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

bool Manager::save(const std::string &path)
{
    auto it = m_configs.find(path);
    if (it == m_configs.end()) {
        return false;
    }

    if (m_userPath.empty()) {
        std::cerr << "config::Manager: cannot save configuration " << path << ": no save path" << std::endl;
        return false;
    }

    std::string pathname = m_userPath + sep + path + ".toml";
    std::string temp = pathname + ".new";
    try {
        std::ofstream f(temp);
        f << it->second.config;
    } catch (std::exception &ex) {
        std::cerr << "config::Manager: failed to save config to " << temp << ": " << ex.what() << std::endl;
        return false;
    }

    if (std::rename(temp.c_str(), pathname.c_str()) != 0) {
        std::cerr << "config::Manager: failed to move updated config to " << pathname << std::endl;
        return false;
    }

    return true;
}

std::ostream &operator<<(std::ostream &os, const ConfigKey &key)
{
    os << key.path << ":" << key.section << ":" << key.name;
    return os;
}

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
