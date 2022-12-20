#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <memory>
#include <map>

#include "entry.h"
#include "config.h"

#include <toml++/toml.h>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

struct Config {
    std::string path;
    toml::table config;
    std::string base;
    bool modified;
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

class Manager {
    friend bool init(const std::string &, int);
    friend bool acquire();
    friend bool release();
    friend void setPrefix(const std::string &);

public:
    static Manager *the();
    static bool exists();
    int rank() const;
    const std::string &cluster() const;

    Config &registerEntry(const EntryBase *entry, const std::string &path, const std::string &section);
    template<class V>
    Entry<V> *getEntry(const std::string &path, const std::string &section, const std::string &name,
                       const V &defaultValue = V());
    template<class V>
    ArrayEntry<V> *getArray(const std::string &path, const std::string &section, const std::string &name);

    bool save(const std::string &path);

private:
    Manager(const std::string &cluster = std::string(), int rank = -1);
    ~Manager();
    void setPrefix(const std::string &dir);
    void acquire();
    bool release();
    void reconfigure();

    const std::string m_cluster;
    const int m_rank = -1;

    int m_useCount = 0;

    std::string m_userPath; // where to write configuration
    std::string m_installPrefix;
    std::vector<std::string> m_path;
    std::map<std::string, Config> m_configs;

    typedef ConfigKey Key;
    std::map<Key, EntryBase *> m_entries;
};

}
#ifdef CONFIG_NAMESPACE
}
#endif
#endif
