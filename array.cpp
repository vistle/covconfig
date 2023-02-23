// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "array.h"
#include "detail/entry.h"
#include "detail/manager.h"
#include "detail/output.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
using namespace detail;

template<class V>
Array<V>::Array(const std::string &path, const std::string &section, const std::string &name, detail::Manager *mgr)
: ConfigBase("Array")
{
    if (!mgr)
        mgr = Manager::the();
    m_entry = mgr->getArray<V>(path, section, name, Flag::Default);
    entry()->addObserver(this);
}

template<class V>
Array<V>::Array(const std::string &path, const std::string &section, const std::string &name,
                const std::vector<V> &value, detail::Manager *mgr, Flag flags)
: ConfigBase("Array")
{
    if (!mgr)
        mgr = Manager::the();
    m_entry = mgr->getArray<V>(path, section, name, flags);
    std::vector<typename detail::VectorStorage<V>::Type> val(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        val[i] = value[i];
    }
    entry()->setOrCheckDefaultValue(val);
    debug() << key() << " initialized to " << entry()->value() << " (default: " << defaultValue() << ")" << std::endl;
    entry()->addObserver(this);
    entry()->assign();
}

template<class V>
Array<V>::Array(ArrayEntry<V> *entry): ConfigBase("Array")
{
    m_entry = entry;
    entry->addObserver(this);
}

template<class V>
Array<V>::~Array()
{
    entry()->removeObserver(this);
}

template<class V>
ArrayEntry<V> *Array<V>::entry() const
{
    return static_cast<ArrayEntry<V> *>(m_entry);
}

template<class V>
size_t Array<V>::size() const
{
    return entry()->size();
}

template<class V>
void Array<V>::resize(size_t size)
{
    return entry()->resize(size);
}

template<class V>
V Array<V>::operator[](size_t index) const
{
    return entry()->at(index);
}

template<class V>
typename Array<V>::ValueProxy &Array<V>::ValueProxy::operator=(const V &value)
{
    assert(array);
    if (array) {
        if (V(array->entry()->at(index)) != value) {
            array->entry()->at(index) = value;
            array->entry()->setModified();
        }
    }
    return *this;
}

template<class V>
typename Array<V>::ValueProxy Array<V>::operator[](size_t index)
{
    if (index >= size()) {
        debug("operator[]") << "resizing from " << size() << " for access at " << index << std::endl;
        resize(index + 1);
    }
    ValueProxy vp{this, index};
    return vp;
}

template<class V>
Array<V>::ValueProxy::ValueProxy(Array<V> *array, size_t index): array(array), index(index)
{}

template<class V>
Array<V>::ValueProxy::~ValueProxy()
{
    array->entry()->store();
}

template<class V>
Array<V> &Array<V>::operator=(const std::vector<V> &val)
{
    if (size() != val.size())
        resize(val.size());
    for (size_t c = 0; c < size(); ++c) {
        if (V(entry()->at(c)) != val[c]) {
            entry()->at(c) = val[c];
            entry()->setModified();
        }
    }
    entry()->store();
    return *this;
}

template<class V>
std::vector<V> Array<V>::value() const
{
    std::vector<V> vec(size());
    for (size_t c = 0; c < size(); ++c) {
        vec[c] = entry()->at(c);
    }
    return vec;
}

template<class V>
std::vector<V> Array<V>::defaultValue() const
{
    std::vector<V> vec(size());
    for (size_t c = 0; c < size(); ++c) {
        vec[c] = entry()->defaultValue().at(c);
    }
    return vec;
}

template<class V>
void Array<V>::update()
{
    auto &str = debug("update");
    str << key() << ": have updater: " << (m_updater ? "yes" : "no");
    str << " {";
    for (auto v: entry()->value())
        str << " " << v;
    str << " }";
    str << std::endl;
    if (m_updater)
        m_updater();
}

template<class V>
void Array<V>::setUpdater(std::function<void()> func)
{
    m_updater = func;
    update();
}


#ifndef WIN32
#undef COVEXPORT
#define COVEXPORT
#endif
template class COVEXPORT Array<bool>;
template class COVEXPORT Array<int64_t>;
template class COVEXPORT Array<double>;
template class COVEXPORT Array<std::string>;

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
