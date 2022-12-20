#ifndef CONFIG_ARRAY_H
#define CONFIG_ARRAY_H

#include <string>
#include "export.h"
#include "observer.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

class EntryBase;
template<class V>
class ArrayEntry;

/// access a homogeneous array of configuration values
/** retrieve, modify and store a homogeneous array of configuration values.
Boolean (`bool`), integral (`int64_t`), floating point (`double`) and string (`std::string`) arrays are supported.
All array members have to be of the same type, even though this is not required by the underlying configuration file.
*/
template<class V>
class Array: private ArrayObserver {
    friend class ValueProxy;

public:
    /// notify configuration subsystem when array members have been modified
    /** proxy class for notifying configuration subsystem of changes to array members. Not meant to be stored by the caller of `operator=` */
    class ValueProxy {
        friend Array;

    public:
        operator V() { return const_cast<const Array &>(*array)[index]; }
        ValueProxy &operator=(const V &value);

    private:
        ValueProxy(Array<V> *array, size_t index): array(array), index(index) {}
        Array *array = nullptr;
        size_t index = 0;
    };

    Array(const std::string &path, const std::string &section, const std::string &name); //< retrieve array
    Array(const std::string &path, const std::string &section, const std::string &name,
          const V &defaultValue); //< retrieve array and fill with `defaultValue` when accessing members beyond size
    virtual ~Array();
    void setUpdater(std::function<void(size_t)> func);
    bool exists() const;
    size_t size() const;
    void resize(size_t size);
    const V &operator[](size_t index) const;
    ValueProxy operator[](size_t index);

private:
    void update(size_t index) override;
    ArrayEntry<V> *m_entry = nullptr;
    std::function<void(size_t)> m_updater;
    bool m_defaultValueValid = false;
    V m_defaultValue = V();
};

extern template class V_CONFIGEXPORT Array<bool>;
extern template class V_CONFIGEXPORT Array<int64_t>;
extern template class V_CONFIGEXPORT Array<double>;
extern template class V_CONFIGEXPORT Array<std::string>;

} // namespace config
#ifdef CONFIG_NAMESPACE
template<class V>
using ConfigArray = config::Array<V>;

typedef ConfigArray<bool> ConfigBoolArray;
typedef ConfigArray<int64_t> ConfigIntArray;
typedef ConfigArray<double> ConfigFloatArray;
typedef ConfigArray<std::string> ConfigStringArray;
}
#endif
#endif
