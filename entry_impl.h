#include "entry.h"
#include "manager.h"

#include <toml++/toml.h>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

template<class V>
Entry<V>::Entry(const std::string &path, const std::string &section, const std::string &name, const V &value)
: ValueEntryBase(path, section, name), m_defaultValue(value)
{
    const int rank = Manager::the()->rank();
    if (rank >= 0) {
        std::string s = m_section + "-" + std::to_string(rank);
        std::cerr << "looking for " << s << std::endl;
        if (auto opt = m_config.config[s][m_name].template value<V>()) {
            m_section = s;
            m_value = *opt;
            m_exists = true;
            return;
        }
    }
    if (auto opt = m_config.config[m_section][m_name].template value<V>()) {
        m_value = *opt;
        m_exists = true;
        return;
    }

    m_exists = false;
    m_value = value;
}

template<class V>
Entry<V>::~Entry()
{
    store();
}

template<class V>
const V &Entry<V>::value() const
{
    return m_value;
}

template<class V>
const V &Entry<V>::defaultValue() const
{
    return m_defaultValue;
}

template<class V>
Entry<V> &Entry<V>::operator=(const V &value)
{
    m_value = value;
    m_modified = true;

    store();

    return *this;
}

template<class V>
void Entry<V>::assign()
{
    *m_config.config[m_section][m_name].template as<V>() = m_value;
}

template<class V>
ArrayEntry<V>::ArrayEntry(const std::string &path, const std::string &section, const std::string &name)
: ArrayEntryBase(path, section, name)
{
    const int rank = Manager::the()->rank();
    if (rank >= 0) {
        std::string s = m_section + "-" + std::to_string(rank);
        std::cerr << "looking for " << s << std::endl;
        if (auto array = m_config.config[s][m_name].as_array()) {
            std::cerr << "array " << s << std::endl;
            m_array = array;
            m_section = s;
            m_exists = true;
            return;
        }
    }
    if (auto array = m_config.config[m_section][m_name].as_array()) {
        m_array = array;
        m_exists = true;
        return;
    }

    m_exists = false;
    if (auto tbl = m_config.config[m_section].as_table()) {
        tbl->insert(m_name, toml::array());
    } else {
        auto tbl2 = toml::table();
        tbl2.insert(m_name, toml::array());
        m_config.config.insert(m_section, tbl2);
    }
    if (auto array = m_config.config[m_section][m_name].as_array()) {
        m_array = array;
        return;
    }
    std::cerr << "could not insert array " << m_section << ":" << m_name << std::endl;
}

template<class V>
ArrayEntry<V>::~ArrayEntry()
{
    store(size());
}

template<class V>
void ArrayEntry<V>::resize(size_t size, const V &value)
{
    m_modified = true;
    assert(m_array);
    m_array->resize(size, value);
    store(size);
}

template<class V>
void ArrayEntry<V>::assign()
{
    assert(m_array);
}

template<class V>
const V &ArrayEntry<V>::at(size_t index) const
{
    assert(m_array);
    return m_array->at(index).template ref<V>();
}

template<class V>
V &ArrayEntry<V>::at(size_t index)
{
    assert(m_array);
    return m_array->at(index).template ref<V>();
}

} // namespace config

#ifdef CONFIG_NAMESPACE
}
#endif
