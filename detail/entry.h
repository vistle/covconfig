// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file entry.h
/// storage shared between Value and Array instances referencing the same item
#pragma once

#include "observer.h"
#include "flags.h"
#include "logger.h"


#include <string>
#include <set>
#include <memory>
#include <vector>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
namespace detail {

struct Config;
class Manager;

template<class V>
struct VectorStorage {
    typedef V Type;
};

template<>
struct VectorStorage<bool> {
    typedef char Type;
};

template<class V>
class ArrayEntry;
template<class V>
class ValueEntry;

class Entry: protected Logger {
public:
    Entry(const std::string &classname, Manager *mgr, const std::string &path, const std::string &section,
          const std::string &name, Flag flags);
    virtual ~Entry();
    bool exists() const;
    virtual void assign() = 0;
    const std::string &path() const;
    const std::string &section() const;
    const std::string &name() const;
    std::string key() const;

    void addObserver(Observer *o);
    void removeObserver(Observer *o);
    void setModified();
    void store();

protected:
    Manager *m_manager = nullptr;
    bool m_modified = false;
    bool m_exists = false;
    const std::string m_path;
    std::string m_section;
    std::string m_name;
    Flag m_flags = Flag::Default;
    Config &m_config;
    std::set<Observer *> m_observers;
};


template<class V>
class EntryBase: public Entry {
    friend class ArrayEntry<V>;
    friend class ValueEntry<V>;

public:
    EntryBase(const std::string &classname, Manager *mgr, const std::string &path, const std::string &section,
              const std::string &name, Flag flags);
    EntryBase &operator=(const V &value);

    const V &value() const;
    const V &defaultValue() const;
    bool setOrCheckDefaultValue(const V &value);

    V m_value = V();
    bool m_defaultValueValid = false;
    V m_defaultValue = V();
};

template<class V>
class ValueEntry: public EntryBase<V> {
public:
    typedef V Type;
    typedef EntryBase<Type> Base;

    ValueEntry(Manager *mgr, const std::string &path, const std::string &section, const std::string &name, Flag flags);
    ~ValueEntry() override;
    void assign() override;

    using Base::operator=;
    using Base::value;
    using Base::defaultValue;
};

template<class V>
class ArrayEntry: public EntryBase<std::vector<typename VectorStorage<V>::Type>> {
public:
    typedef typename VectorStorage<V>::Type Type;
    typedef EntryBase<std::vector<Type>> Base;

    ArrayEntry(Manager *mgr, const std::string &path, const std::string &section, const std::string &name, Flag flags);
    ~ArrayEntry() override;
    void assign() override;

    using Base::operator=;
    using Base::value;
    using Base::defaultValue;

    size_t size() const;
    void resize(size_t size, const V &value = V());
    Type &at(size_t index);
    const Type &at(size_t index) const;
};

extern template class ValueEntry<bool>;
extern template class ValueEntry<int64_t>;
extern template class ValueEntry<double>;
extern template class ValueEntry<std::string>;
extern template class ArrayEntry<bool>;
extern template class ArrayEntry<int64_t>;
extern template class ArrayEntry<double>;
extern template class ArrayEntry<std::string>;

} // namespace detail
} // namespace config

#ifdef CONFIG_NAMESPACE
}
#endif
