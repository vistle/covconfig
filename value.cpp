// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "value.h"
#include "detail/entry.h"
#include "detail/manager.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
using namespace detail;

template<class V>
Value<V>::Value(const std::string &path, const std::string &section, const std::string &name, const V &value,
                Manager *mgr, Flag flags)
: ConfigBase("Value")
{
    if (!mgr)
        mgr = Manager::the();
    m_entry = mgr->getValue<V>(path, section, name, flags);
    entry()->setOrCheckDefaultValue(value);
    debug() << key() << " initialized to " << entry()->value() << " (default: " << defaultValue() << ")" << std::endl;
    entry()->addObserver(this);
    entry()->assign();
}

template<class V>
Value<V>::Value(const std::string &path, const std::string &section, const std::string &name, Manager *mgr)
: ConfigBase("Value")
{
    if (!mgr)
        mgr = Manager::the();
    m_entry = mgr->getValue<V>(path, section, name, Flag::Default);
    entry()->checkDefaultValue();
    entry()->addObserver(this);
}

template<class V>
Value<V>::Value(ValueEntry<V> *entry): ConfigBase("Value")
{
    m_entry = entry;
    entry->addObserver(this);
}

template<class V>
Value<V>::~Value()
{
    entry()->removeObserver(this);
}

template<class V>
ValueEntry<V> *Value<V>::entry() const
{
    return static_cast<ValueEntry<V> *>(m_entry);
}

template<class V>
const V &Value<V>::defaultValue() const
{
    return entry()->defaultValue();
}

template<class V>
const V &Value<V>::value() const
{
    return entry()->value();
}

template<class V>
Value<V>::operator V() const
{
    return value();
}

template<class V>
Value<V> &Value<V>::operator=(const V &value)
{
    *entry() = value;
    return *this;
}

template<class V>
void Value<V>::update()
{
    debug("update") << key() << ": have updater: " << (m_updater ? "yes" : "no") << ", value=" << entry()->value()
                    << std::endl;
    if (m_updater)
        m_updater(entry()->value());
}

template<class V>
void Value<V>::setUpdater(std::function<void(const V &)> func)
{
    m_updater = func;
    update();
}

#ifndef WIN32
#undef COVEXPORT
#define COVEXPORT
#endif
template class COVEXPORT Value<bool>;
template class COVEXPORT Value<int64_t>;
template class COVEXPORT Value<double>;
template class COVEXPORT Value<std::string>;
template class COVEXPORT Value<config::Section>;

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
