#include "value.h"
#include "entry.h"
#include "manager.h"
#include "manager_impl.h"
#include "entry_impl.h"

#include <toml++/toml.h>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

template<class V>
Value<V>::Value(const std::string &path, const std::string &section, const std::string &name, const V &value)
{
    m_entry = Manager::the()->getEntry(path, section, name, value);
    m_entry->addObserver(this);
}

template<class V>
Value<V>::~Value()
{
    m_entry->removeObserver(this);
}

template<class V>
bool Value<V>::exists() const
{
    return m_entry->exists();
}

template<class V>
const V &Value<V>::value() const
{
    return m_entry->value();
}

template<class V>
Value<V>::operator V() const
{
    return value();
}

template<class V>
Value<V> &Value<V>::operator=(const V &value)
{
    *m_entry = value;
    return *this;
}

template<class V>
void Value<V>::update()
{
    std::cerr << "update: val = " << m_entry->value() << std::endl;
    if (m_updater)
        m_updater(m_entry->value());
}

template<class V>
void Value<V>::setUpdater(std::function<void(const V &)> func)
{
    m_updater = func;
    update();
}


template class Value<bool>;
template class Value<int64_t>;
template class Value<double>;
template class Value<std::string>;

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
