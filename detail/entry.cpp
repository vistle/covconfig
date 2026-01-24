// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "entry.h"
#include "manager.h"
#include "output.h"
#include "../value.h"
#include "../array.h"
#include <mutex>

#include <toml++/toml.h>

#include <cassert>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
namespace detail {

Entry::Entry(const std::string &classname, Manager *mgr, const std::string &path, const std::string &section,
             const std::string &name, Flag flags)
: Logger(classname)
, m_manager(mgr)
, m_path(path)
, m_section(section)
, m_name(name)
, m_flags(flags)
, m_config(mgr->registerPath(path))
{
    bool secInvalid = m_section.find(' ') != std::string::npos || m_section.find('-') != std::string::npos;
    bool nameInvalid = m_name.find(' ') != std::string::npos || m_name.find('-') != std::string::npos ||
                       m_name.find('.') != std::string::npos;
    if (secInvalid) {
        error() << key() << " contains invalid character in section: " << m_section << std::endl;
    }
    if (nameInvalid) {
        error() << key() << " contains invalid character in name: " << m_name << std::endl;
    }
    assert(m_section.find(' ') == std::string::npos);
    assert(m_section.find('-') == std::string::npos); // reserved for rank suffix
    assert(m_name.find(' ') == std::string::npos);
    assert(m_name.find('-') == std::string::npos); // reserved for rank suffix
    assert(m_name.find('.') == std::string::npos); // would confuse mechanism where rank is inserted
}


Entry::~Entry()
{}

bool Entry::exists() const
{
    return m_exists;
}

const std::string &Entry::path() const
{
    return m_path;
}

const std::string &Entry::section() const
{
    return m_section;
}

const std::string &Entry::name() const
{
    return m_name;
}

std::string Entry::key() const
{
    std::string k;
    k += path();
    k += ":";
    k += section();
    k += ":";
    k += name();
    return k;
}

void Entry::setModified()
{
    m_modified = true;
}

void Entry::store()
{
    if (m_modified) {
        assign();
        m_modified = false;
        debug("store") << key() << ", notifying " << m_observers.size() << " observers" << std::endl;
        for (auto *o: m_observers) {
            o->update();
        }
    }
}

Flag Entry::flags() const
{
    return m_flags;
}

void Entry::addObserver(Observer *o)
{
    m_observers.emplace(o);
    debug("addObserver") << key() << ", now " << m_observers.size() << " observers" << std::endl;
}

void Entry::removeObserver(Observer *o)
{
    m_observers.erase(o);
    debug("removeObserver") << key() << ", now " << m_observers.size() << " observers" << std::endl;
}


template<class V>
EntryBase<V>::EntryBase(const std::string &classname, Manager *mgr, const std::string &path, const std::string &section,
                        const std::string &name, Flag flags)
: Entry(classname, mgr, path, section, name, flags)
{}

template<class V>
ValueEntry<V>::ValueEntry(Manager *mgr, const std::string &path, const std::string &section, const std::string &name,
                          Flag flags)
: EntryBase<V>("ValueEntry", mgr, path, section, name, flags)
{
    const int rank = this->m_manager->rank();
    if (rank >= 0) {
        std::string s = this->m_section + "-" + std::to_string(rank);
        this->debug() << "looking for value " << s << std::endl;
        if (auto opt = this->m_config.config[s][this->m_name].template value<V>()) {
            this->m_exists = true;
            this->m_section = s;
            this->m_value = *opt;
            return;
        }
    }
    this->debug() << "searching in " << this->m_section << ":" << this->m_name << std::endl;
    if (auto opt = this->m_config.config[this->m_section][this->m_name].template value<V>()) {
        //debug() << m_config.config << std::endl;
        this->m_exists = true;
        this->m_value = *opt;
        this->debug() << "FOUND " << this->m_section << ":" << this->m_name << ": value=" << this->m_value << std::endl;
        return;
    }
}

template<class V>
V ValueEntry<V>::overrideDefaultValue(const V &value, bool &valid)
{
    valid = false;
    if (auto opt = this->m_config.defaultOverrides[this->m_section][this->m_name].template value<V>()) {
        valid = true;
        return *opt;
    }
    return value;
}

template<class V>
ValueEntry<V>::~ValueEntry()
{
    this->store();
}

template<class V>
std::unique_ptr<ConfigBase> ValueEntry<V>::create()
{
    return std::unique_ptr<ConfigBase>(new Value<V>(this));
}

template<class V>
const V &EntryBase<V>::value() const
{
    return m_value;
}

template<class V>
const V &EntryBase<V>::defaultValue() const
{
    return m_defaultValue;
}

template<class V>
bool EntryBase<V>::hasDefaultValue() const
{
    return m_defaultValueValid;
}

template<class V>
bool EntryBase<V>::checkDefaultValue()
{
    bool overrideValid = false;
    auto val = overrideDefaultValue(V(), overrideValid);
    if (overrideValid) {
        debug("checkDefaultValue") << key() << ": overridden default value: " << val << std::endl;
        return setOrCheckDefaultValue(V());
    }
    return false;
}

template<class V>
bool EntryBase<V>::setOrCheckDefaultValue(const V &value)
{
    bool overrideValid = false;
    auto val = overrideDefaultValue(value, overrideValid);
    if (overrideValid) {
        debug("setOrCheckDefaultValue") << key() << ": overridden default value: " << val << std::endl;
    }
    if (!m_defaultValueValid) {
        m_defaultValue = val;
        m_defaultValueValid = true;
        debug("setOrCheckDefaultValue") << key() << ": default: " << defaultValue() << std::endl;
    } else if (val != m_defaultValue) {
        error("setOrCheckDefaultValue") << "differing default values for " << key() << ": " << defaultValue()
                                        << " is registered, request is " << val << std::endl;
        m_manager->handleError();
        return false;
    }

    if (!exists()) {
        debug("setOrCheckDefaultValue") << key() << " does not exist, updating initial value from " << this->value()
                                        << " to " << val << std::endl;
        m_value = val;
    }

    return true;
}

template<class V>
EntryBase<V> &EntryBase<V>::operator=(const V &value)
{
    if (m_value != value) {
        m_value = value;
        setModified();
    }
    store();

    return *this;
}

template<class V>
void ValueEntry<V>::assign()
{
    if (this->m_flags == Flag::PerModel) {
        Value<V> val(this);
        if (this->m_manager->sendToWorkspace(&val)) {
            this->debug("assign") << this->key() << " sent to workspace" << std::endl;
        } else {
            this->warn("assign") << "could not send " << this->key() << " to workspace" << std::endl;
        }
        return;
    }

    auto tbl = this->m_config.config[this->m_section].as_table();
    if (!tbl) {
        this->m_config.config.insert(this->m_section, toml::table());
        tbl = this->m_config.config[this->m_section].as_table();
        if (!tbl) {
            this->error("assign") << "name=" << this->m_name << ", could not insert parent table for section "
                                  << this->m_section << " at " << this->m_path << std::endl;
            return;
        }
    }
    if (this->m_defaultValueValid && this->m_value == this->m_defaultValue) {
        tbl->erase(this->m_name);
        this->debug("assign") << this->key() << ", " << this->m_value << " is default, erased from toml" << std::endl;
    } else {
        tbl->insert_or_assign(this->m_name, this->m_value);
        this->debug("assign") << this->key() << " inserted/assigned " << this->m_value << " to toml" << std::endl;
    }
    this->m_config.modified = true;
}

template<class V>
ArrayEntry<V>::ArrayEntry(Manager *mgr, const std::string &path, const std::string &section, const std::string &name,
                          Flag flags)
: EntryBase<std::vector<typename ArrayEntry<V>::Type>>("ArrayEntry", mgr, path, section, name, flags)
{
    const toml::array *array = nullptr;
    const int rank = this->m_manager->rank();
    if (rank >= 0) {
        std::string s = this->m_section + "-" + std::to_string(rank);
        array = this->m_config.config[s][this->m_name].as_array();
        if (array)
            this->m_section = s;
    }
    if (!array) {
        array = this->m_config.config[this->m_section][this->m_name].as_array();
    }

    if (array) {
        for (auto &v: *array) {
            if (auto vv = v.as<V>()) {
                this->m_value.push_back(vv->get());
            } else {
                this->m_value.clear();
                this->warn() << this->key() << ": array not convertible to requested type" << std::endl;
                return;
            }
        }
        this->m_exists = true;
    }
}

template<class V>
typename ArrayEntry<V>::ArrayType ArrayEntry<V>::overrideDefaultValue(const typename ArrayEntry<V>::ArrayType &value,
                                                                      bool &valid)
{
    valid = false;
    if (auto array = this->m_config.defaultOverrides[this->m_section][this->m_name].as_array()) {
        typename ArrayEntry<V>::ArrayType result;
        for (auto &v: *array) {
            if (auto vv = v.template as<V>()) {
                result.push_back(vv->get());
            } else {
                this->warn() << this->key() << ": array not convertible to requested type" << std::endl;
                return value;
            }
        }
        valid = true;
        return result;
    }
    return value;
}


template<class V>
ArrayEntry<V>::~ArrayEntry()
{
    this->store();
}

template<class V>
std::unique_ptr<ConfigBase> ArrayEntry<V>::create()
{
    return std::unique_ptr<ConfigBase>(new Array<V>(this));
}

template<class V>
void ArrayEntry<V>::assign()
{
    if (this->m_flags == Flag::PerModel) {
        Array<V> arr(this);
        this->m_manager->sendToWorkspace(&arr);
        return;
    }

    std::lock_guard guard(this->m_config.mutex);
    auto tbl = this->m_config.config[this->m_section].as_table();
    if (!tbl) {
        this->m_config.config.insert(this->m_section, toml::table());
        tbl = this->m_config.config[this->m_section].as_table();
        if (!tbl) {
            this->error("assign") << "name=" << this->m_name << ", could not insert parent table for section "
                                  << this->m_section << " at " << this->m_path << std::endl;
            return;
        }
    }
    if (this->m_defaultValueValid && this->m_value == this->m_defaultValue) {
        tbl->erase(this->m_name);
        this->debug("assign") << this->key() << ", " << this->m_value << " is default, erased from toml" << std::endl;
    } else {
        toml::array array;
        for (auto &v: this->m_value) {
            array.push_back(V(v));
        }
        tbl->insert_or_assign(this->m_name, array);
        this->debug("assign") << this->key() << " inserted/assigned " << this->m_value << " to toml" << std::endl;
    }
    this->m_config.modified = true;
}

template<class V>
size_t ArrayEntry<V>::size() const
{
    return this->m_value.size();
}

template<class V>
void ArrayEntry<V>::resize(size_t size, const V &value)
{
    if (this->m_value.size() != size) {
        this->m_value.resize(size, value);
        this->setModified();
    }
}

template<class V>
typename ArrayEntry<V>::Type &ArrayEntry<V>::at(size_t index)
{
    return this->m_value.at(index);
}

template<class V>
const typename ArrayEntry<V>::Type &ArrayEntry<V>::at(size_t index) const
{
    return this->m_value.at(index);
}

template class EntryBase<bool>;
template class EntryBase<int64_t>;
template class EntryBase<double>;
template class EntryBase<std::string>;

template class EntryBase<std::vector<char>>;
template class EntryBase<std::vector<int64_t>>;
template class EntryBase<std::vector<double>>;
template class EntryBase<std::vector<std::string>>;


template class ValueEntry<bool>;
template class ValueEntry<int64_t>;
template class ValueEntry<double>;
template class ValueEntry<std::string>;

template class ArrayEntry<bool>;
template class ArrayEntry<int64_t>;
template class ArrayEntry<double>;
template class ArrayEntry<std::string>;

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
