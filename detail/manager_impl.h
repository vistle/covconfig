// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "manager.h"
#include "entry.h"
#include "output.h"

#include <iostream>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
namespace detail {

template<class V>
ValueEntry<V> *Manager::getValue(const std::string &path, const std::string &section, const std::string &name,
                                 Flag flags)
{
    Key key{path, section, name};
    auto it = m_entries.find(key);
    if (it == m_entries.end()) {
        it = m_entries.emplace(key, new ValueEntry<V>(this, path, section, name, flags)).first;
        auto ent = dynamic_cast<ValueEntry<V> *>(it->second);
        debug("getValue") << key << " new, value: " << ent->value() << std::endl;
        return ent;
    }

    auto ent = dynamic_cast<ValueEntry<V> *>(it->second);
    if (!ent) {
        error("getValue") << key << " already registered with a different type" << std::endl;
        abort();
    }
    debug("getValue") << key << " found, existing value: " << ent->value() << ", default: " << ent->defaultValue()
                      << std::endl;
    return ent;
}

template<class V>
ArrayEntry<V> *Manager::getArray(const std::string &path, const std::string &section, const std::string &name,
                                 Flag flags)
{
    Key key{path, section, name};
    auto it = m_entries.find(key);
    if (it == m_entries.end()) {
        it = m_entries.emplace(key, new ArrayEntry<V>(this, path, section, name, flags)).first;
        auto ent = dynamic_cast<ArrayEntry<V> *>(it->second);
        debug("getArray") << key << " new, array: " << ent->value() << std::endl;
        return ent;
    }

    auto ent = dynamic_cast<ArrayEntry<V> *>(it->second);
    if (!ent) {
        error("getArray") << key << " already registered with a different type" << std::endl;
        abort();
    }
    debug("getArray") << key << " found, existing array: " << ent->value() << ", default: " << ent->defaultValue()
                      << std::endl;
    return ent;
}

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
