// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file flag.h
/// setting flags enum
#pragma once

#include <string>
#include <type_traits>
#include "export.h"
#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

enum class COVEXPORT Flag {
    Default = 0,
    PerModel = 1,
};
namespace detail {
template<class V>
class ValueEntry;
#define FOR_ALL_CONFIG_TYPES(code) code(bool) code(int64_t) code(double) code(std::string)

template<class V>
struct isValueType {
    static constexpr bool value = false;
};

#define VALUE_TYPE_DECL(V) \
    template<> \
    struct isValueType<V> { \
        static constexpr bool value = true; \
    };

FOR_ALL_CONFIG_TYPES(VALUE_TYPE_DECL)
} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
