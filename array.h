// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file array.h
/// provide access to arrays of uniform configuration values
#pragma once

#include <string>
#include <functional>
#include "detail/export.h"
#include "detail/flags.h"
#include "detail/base.h"
#include "section.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {
class Manager;
/// opaque storage for \ref Array's of type V
template<class V>
class ArrayEntry;
} // namespace detail

/// access a homogeneous array of configuration values
/** retrieve, modify and store a homogeneous array of configuration values.
Boolean (`bool`), integral (`int64_t`), floating point (`double`) and string (`std::string`) arrays are supported.
All array members have to be of the same type, even though this is not required by the underlying configuration file.
*/
template<class V>
class Array: public ConfigBase {
    friend class detail::ArrayEntry<V>;
    friend class ValueProxy;

public:
    /// notify configuration subsystem when array members have been modified
    /** proxy class for notifying configuration subsystem of changes to array members. Not meant to be stored by the caller of \ref Array::operator[] */
    class COVEXPORT ValueProxy {
        friend Array;

    public:
        operator V() { return const_cast<const Array &>(*array)[index]; } ///< retrieve value
        ValueProxy &operator=(
            const V &
                value); ///< assign new value to entry and notify other instances of \ref Array referencing the same data

        ~ValueProxy();

    private:
        ValueProxy(Array<V> *array, size_t index);
        Array *array = nullptr;
        size_t index = 0;
    };

    Array() = delete;
    Array(const Array &other) =
        delete; ///< removed as copying does not work well when functor objects are provided via \ref setUpdater
    Array(const std::string &path, const std::string &section, const std::string &name,
          detail::Manager *mgr = nullptr); ///< retrieve existing array via Manager mgr (or the default manager)
    Array(const std::string &path, const std::string &section, const std::string &name, const std::vector<V> &value,
          detail::Manager *mgr = nullptr,
          Flag flags = Flag::Default); ///< retrieve array or initialize to `value`
    ~Array() override;
    void setUpdater(std::function<void()> func); ///< set `func` to be notified when size or an entry changes
    size_t size() const; ///< retrieve number of values
    void resize(
        size_t
            size); ///< change number of values, newly created entries will be set to the default value provided at construction time
    V operator[](size_t index) const; ///< retrieve value with @param index
    ValueProxy operator[](size_t index); ///< change value of @param index
    Array &operator=(const std::vector<V> &val); ///< assign array of values
    std::vector<V> value() const; ///< retrieve all values
    std::vector<V> defaultValue() const; ///< retrieve default values

private:
    Array(detail::ArrayEntry<V> *entry); ///< create from a provided existing entry where data is stored

    void update() override; ///< called when size or value changes
    detail::ArrayEntry<V> *entry() const; ///< access storage of values
    std::function<void()> m_updater; ///< change listener
};
#ifndef WIN32
extern template class COVEXPORT Array<bool>; ///< instantiated type
extern template class COVEXPORT Array<int64_t>; ///< instantiated type
extern template class COVEXPORT Array<double>; ///< instantiated type
extern template class COVEXPORT Array<std::string>; ///< instantiated type
extern template class COVEXPORT Array<config::Section>; ///< instantiated type
#endif

} // namespace config
#ifdef CONFIG_NAMESPACE
template<class V>
using ConfigArray = config::Array<V>; ///< bring into provided namespace

typedef ConfigArray<bool> ConfigBoolArray; ///< convenience typedef
typedef ConfigArray<int64_t> ConfigIntArray; ///< convenience typedef
typedef ConfigArray<double> ConfigFloatArray; ///< convenience typedef
typedef ConfigArray<std::string> ConfigStringArray; ///< convenience typedef
typedef ConfigArray<config::Section> ConfigSectionArray; ///< convenience typedef
}
#endif
