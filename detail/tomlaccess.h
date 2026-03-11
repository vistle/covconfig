// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file tomlaccess.h
/// utility functions for accessing TOML++ tables

#pragma once

#include <optional>
#include <toml++/toml.h>
#include "../section.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

class Section;

namespace detail {

class Logger;

toml::table *table_for_section(const Logger &logger, toml::table &root, const std::string &section,
                               bool create = false);
const toml::table *table_for_section(const Logger &logger, const toml::table &root, const std::string &section);

class Entry;

template<class V>
struct Convert {
    typedef V Type;
    typedef V TomlType;

    static TomlType to_toml(Entry *entry, const V &v);
    static std::optional<Type> get_from_table(Entry *entry, const toml::table *tbl, const std::string &name,
                                              bool optional = false);
    static std::optional<Type> as(Entry *entry, size_t index, const toml::node &n);
};

extern template struct Convert<bool>;
extern template struct Convert<int64_t>;
extern template struct Convert<double>;
extern template struct Convert<std::string>;

template<>
struct Convert<Section> {
    typedef Section Type;
    typedef toml::table TomlType;

    static TomlType to_toml(Entry *entry, const Type &v);
    static std::optional<Type> get_from_table(Entry *entry, const toml::table *tbl, const std::string &name,
                                              bool optional = false);
    static const std::optional<Type> as(Entry *entry, size_t index, const toml::node &n);
};

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
