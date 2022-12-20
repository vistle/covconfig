#include "array.h"
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
Array<V>::Array(const std::string &path, const std::string &section, const std::string &name)
{
    m_entry = Manager::the()->getArray<V>(path, section, name);
    m_entry->addObserver(this);
}

template<class V>
Array<V>::Array(const std::string &path, const std::string &section, const std::string &name, const V &defaultValue)
: m_defaultValueValid(true), m_defaultValue(defaultValue)
{
    m_entry = Manager::the()->getArray<V>(path, section, name);
    m_entry->addObserver(this);
}

template<class V>
Array<V>::~Array()
{
    m_entry->removeObserver(this);
}

template<class V>
bool Array<V>::exists() const
{
    return m_entry->exists();
}

template<class V>
size_t Array<V>::size() const
{
    return m_entry->size();
}

template<class V>
void Array<V>::resize(size_t size)
{
    return m_entry->resize(size, m_defaultValue);
}

template<class V>
const V &Array<V>::operator[](size_t index) const
{
    if (m_defaultValueValid && index >= size())
        return m_defaultValue;
    return m_entry->at(index);
}

template<class V>
typename Array<V>::ValueProxy &Array<V>::ValueProxy::operator=(const V &value)
{
    array->m_entry->at(index) = value;
    array->m_entry->store(index);
    return *this;
}

template<class V>
typename Array<V>::ValueProxy Array<V>::operator[](size_t index)
{
    if (index >= size()) {
        resize(index + 1);
    }
    ValueProxy vp{this, index};
    return vp;
}

template<class V>
void Array<V>::update(size_t index)
{
    std::cerr << "update@" << index << ", size=" << size();
    if (index < size()) {
        std::cerr << ", val=" << m_entry->at(index);
    }
    std::cerr << std::endl;
    if (m_updater)
        m_updater(index);
}

template<class V>
void Array<V>::setUpdater(std::function<void(size_t)> func)
{
    m_updater = func;
    update(size());
}


template class Array<bool>;
template class Array<int64_t>;
template class Array<double>;
template class Array<std::string>;

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
