#include "manager.h"
#include "entry.h"

#include <iostream>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

template<class V>
Entry<V> *Manager::getEntry(const std::string &path, const std::string &scope, const std::string &name,
                            const V &defaultValue)
{
    Key key{path, scope, name};
    auto it = m_entries.find(key);
    if (it == m_entries.end()) {
        it = m_entries.emplace(key, new Entry(path, scope, name, defaultValue)).first;
        return static_cast<Entry<V> *>(it->second);
    }

    auto ent = dynamic_cast<Entry<V> *>(it->second);
    if (!ent) {
        std::cerr << "config::Manager: " << path << ":" << scope << ":" << name
                  << " already registered with a different type" << std::endl;
        abort();
    }
    if (ent->defaultValue() != defaultValue) {
        std::cerr << "config::Manager: differing default values for " << path << ":" << scope << ":" << name << ": "
                  << ent->defaultValue() << " is registered, request is " << defaultValue << std::endl;
    }
    return ent;
}

template<class V>
ArrayEntry<V> *Manager::getArray(const std::string &path, const std::string &scope, const std::string &name)
{
    Key key{path, scope, name};
    auto it = m_entries.find(key);
    if (it == m_entries.end()) {
        std::cerr << "config::Manager: new array " << key << std::endl;
        it = m_entries.emplace(key, new ArrayEntry<V>(path, scope, name)).first;
        return static_cast<ArrayEntry<V> *>(it->second);
    }

    auto ent = dynamic_cast<ArrayEntry<V> *>(it->second);
    if (!ent) {
        std::cerr << "config::Manager: " << path << ":" << scope << ":" << name
                  << " already registered with a different type" << std::endl;
        abort();
    }
    std::cerr << "config::Manager: reusing array " << key << std::endl;
    return ent;
}

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
