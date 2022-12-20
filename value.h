#ifndef CONFIG_VALUE_H
#define CONFIG_VALUE_H

#include <string>
#include "export.h"
#include "observer.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

class EntryBase;
template<class V> class Entry;

/// access an individual configuration value
/** retrieve, modify and store an individual configuration value.
Boolean (`bool`), integral (`int64_t`), floating point (`double`) and string (`std::string`) data is supported.
*/
template<class V>
class Value: private ValueObserver {
public:
    Value(const std::string &path, const std::string &section, const std::string &name, const V &value = V());
    virtual ~Value();
    void setUpdater(std::function<void(const V &)> func);
    bool exists() const;
    const V &value() const;
    operator V() const;
    Value &operator=(const V &value);

private:
    void update() override;
    Entry<V> *m_entry = nullptr;
    std::function<void(const V &)> m_updater;
};

extern template class V_CONFIGEXPORT Value<bool>;
extern template class V_CONFIGEXPORT Value<int64_t>;
extern template class V_CONFIGEXPORT Value<double>;
extern template class V_CONFIGEXPORT Value<std::string>;

}
#ifdef CONFIG_NAMESPACE
template<class V>
using ConfigValue = config::Value<V>;

typedef ConfigValue<bool> ConfigBool;
typedef ConfigValue<int64_t> ConfigInt;
typedef ConfigValue<double> ConfigFloat;
typedef ConfigValue<std::string> ConfigString;

}
#endif
#endif
