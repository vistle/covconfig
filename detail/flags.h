// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file flags.h
/// setting flags enum
#pragma once

#include <string>
#include <type_traits>
#include "export.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

class Section;

enum class COVEXPORT Flag {
    Default = 0,
    PerModel = 1,
};

namespace detail {
template<class V>
class ValueEntry;
#define COVCONFIG_FOR_ALL_CONFIG_TYPES(code) code(bool) code(int64_t) code(double) code(std::string) code(Section)

#ifndef DOXYGEN
template<class V>
struct isValueType {
    static constexpr bool value = false;
};

#define VALUE_TYPE_DECL(V) \
    template<> \
    struct isValueType<V> { \
        static constexpr bool value = true; \
    };

COVCONFIG_FOR_ALL_CONFIG_TYPES(VALUE_TYPE_DECL)
#undef COVCONFIG_FOR_ALL_CONFIG_TYPES

} // namespace detail

template<class V>
class Value;
template<class V>
class Array;

//enable value functions only for supported non-array types
#endif
template<class V>
using ValuePtr = std::enable_if_t<detail::isValueType<V>::value,
                                  std::unique_ptr<Value<V>>>; ///< unique pointer to a configuration value
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
