#include "entry.h"
#include "manager.h"
#include "manager_impl.h"
#include "entry_impl.h"

#include <toml++/toml.h>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

EntryBase::EntryBase(const std::string &path, const std::string &section, const std::string &name)
: m_path(path), m_section(section), m_name(name), m_config(Manager::the()->registerEntry(this, path, section))
{
    assert(m_section.find(' ') == std::string::npos);
    assert(m_section.find('-') == std::string::npos); // reserved for rank suffix
    assert(m_name.find(' ') == std::string::npos);
    assert(m_name.find('-') == std::string::npos); // reserved for rank suffix
    assert(m_name.find('.') == std::string::npos); // would confuse mechanism where rank is inserted
}


EntryBase::~EntryBase()
{}

bool EntryBase::exists() const
{
    return m_exists;
}

void ValueEntryBase::store()
{
    if (m_modified) {
        assign();
        m_config.modified = true;
        m_modified = false;
        for (auto *o: m_observers) {
            o->update();
        }
    }
}

void ValueEntryBase::addObserver(ValueObserver *o)
{
    m_observers.emplace(o);
}

void ValueEntryBase::removeObserver(ValueObserver *o)
{
    m_observers.erase(o);
}

void ArrayEntryBase::addObserver(ArrayObserver *o)
{
    m_observers.emplace(o);
}

void ArrayEntryBase::removeObserver(ArrayObserver *o)
{
    m_observers.erase(o);
}

void ArrayEntryBase::store(size_t index)
{
    if (m_modified) {
        assign();
        m_config.modified = true;
        m_modified = false;
    }
    for (auto *o: m_observers) {
        o->update(index);
    }
}

size_t ArrayEntryBase::size() const
{
    assert(m_array);
    return m_array->size();
}

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
