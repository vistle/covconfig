// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file value.h
/// provide access to a single configuration value
#pragma once

#include <string>
#include <functional>
#include "detail/export.h"
#include "detail/flags.h"
#include "detail/base.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {
class Manager;
/// opaque storage for \ref Value's of type V
template<class V>
class ValueEntry;
} // namespace detail

/// access an individual configuration value
/** retrieve, modify and store an individual configuration value.
Boolean (`bool`), integral (`int64_t`), floating point (`double`) and string (`std::string`) data is supported.
*/
template<class V>
class Value: public ConfigBase {
    friend class detail::ValueEntry<V>;

public:
    Value() = delete;
    Value(const Value &other) =
        delete; ///< removed as copying does not work well when functor objects are provided via \ref setUpdater
    Value(const std::string &path, const std::string &section, const std::string &name,
          detail::Manager *mgr = nullptr); ///< create from an existing entry managed by mgr (or the default manager)
    Value(const std::string &path, const std::string &section, const std::string &name, const V &value,
          detail::Manager *mgr = nullptr,
          Flag flags =
              Flag::Default); ///< create new entry with default value, must match default and flags at other locations
    ~Value() override;
    void setUpdater(std::function<void(const V &)> func); ///< set `func` to be notified when value changes
    const V &value() const; ///< retrieve value
    const V &defaultValue() const; ///< retrieve default value
    operator V() const; ///< retrieve value
    Value &operator=(const V &value); ///< assign a new value

private:
    Value(detail::ValueEntry<V> *entry); ///< create from a provided existing entry where data is stored

    void update() override; ///< called when value is changed
    detail::ValueEntry<V> *entry() const; ///< access storage of value
    std::function<void(const V &)> m_updater; ///< change listener
};
#ifndef WIN32
extern template class COVEXPORT Value<bool>; ///< instantiated type
extern template class COVEXPORT Value<int64_t>; ///< instantiated type
extern template class COVEXPORT Value<double>; ///< instantiated type
extern template class COVEXPORT Value<std::string>; ///< instantiated type
#endif
}
#ifdef CONFIG_NAMESPACE
template<class V>
using ConfigValue = config::Value<V>; ///< bring into provided namespace

typedef ConfigValue<bool> ConfigBool; ///< convenience typedef
typedef ConfigValue<int64_t> ConfigInt; ///< convenience typedef
typedef ConfigValue<double> ConfigFloat; ///< convenience typedef
typedef ConfigValue<std::string> ConfigString; ///< convenience typedef
}
#endif
