#ifndef CONFIG_ENTRY_H
#define CONFIG_ENTRY_H

#include <string>
#include <set>
#include <memory>

#include "observer.h"

#include <toml++/toml.h>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

struct Config;

class EntryBase
{
public:
    EntryBase(const std::string &path, const std::string &section, const std::string &name);
    virtual ~EntryBase();
    bool exists() const;
    virtual void assign() = 0;

protected:
    bool m_modified = false;
    bool m_exists = false;
    const std::string m_path;
    std::string m_section;
    std::string m_name;
    Config &m_config;
};

class ValueEntryBase: public EntryBase {
public:
    using EntryBase::EntryBase;
    //ValueEntryBase(const std::string &path, const std::string &section, const std::string &name);
    void addObserver(ValueObserver *o);
    void removeObserver(ValueObserver *o);
    void store();

protected:
    std::set<ValueObserver *> m_observers;
};

template<class V>
class Entry: public ValueEntryBase {
public:
    Entry(const std::string &path, const std::string &section, const std::string &name, const V &value = V());
    virtual ~Entry();
    Entry &operator=(const V &value);

    const V &value() const;
    const V &defaultValue() const;
    void assign() override;

private:
    V m_value = V();
    const V m_defaultValue = V();
};

class ArrayEntryBase: public EntryBase {
public:
    using EntryBase::EntryBase;
    //ArrayEntryBase(const std::string &path, const std::string &section, const std::string &name);
    size_t size() const;
    void addObserver(ArrayObserver *o);
    void removeObserver(ArrayObserver *o);
    void store(size_t index);

protected:
    std::set<ArrayObserver *> m_observers;
    toml::array *m_array = nullptr;
};

template<class V>
class ArrayEntry: public ArrayEntryBase {
public:
    ArrayEntry(const std::string &path, const std::string &section, const std::string &name);
    virtual ~ArrayEntry();
    V &at(size_t index);
    const V &at(size_t index) const;

    void assign() override;
    void resize(size_t size, const V &value = V());

private:
};
}

#ifdef CONFIG_NAMESPACE
}
#endif
#endif
