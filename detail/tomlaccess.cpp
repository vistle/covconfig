// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "tomlaccess.h"
#include "entry.h"
#include "logger.h"

#include <toml++/toml.h>

#include <cassert>
#include <iostream>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
namespace detail {

toml::node *node_for_path(const Logger &logger, toml::table &root, const std::string &path, bool create)
{
    if (path.empty()) {
        return &root;
    }

    auto dot = path.find('.');
    if (dot == std::string::npos) {
        auto bracket = path.find('[');
        if (bracket != std::string::npos) {
            if (path.back() != ']') {
                logger.error("-> node_for_path") << "invalid section name " << path << std::endl;
                return nullptr;
            }
            auto index = path.substr(bracket + 1, path.size() - bracket - 2);
            auto array = path.substr(0, bracket);
            auto node = root[array];
            if (!node) {
                if (!create)
                    return nullptr;
                root.insert(array, toml::array());
                node = root[array];
            }
            auto arr = node.as_array();
            if (!arr) {
                logger.error("-> node_for_path")
                    << "expected array for section " << array << " in table " << root << std::endl;
                return nullptr;
            }
            auto idx = std::stoul(index);
            if (idx < 0) {
                logger.error("-> node_for_path")
                    << "negative index " << idx << " for array " << array << " in table " << root << std::endl;
                return nullptr;
            }
            if (idx >= arr->size()) {
                if (!create) {
                    logger.error("-> node_for_path")
                        << "index " << idx << " out of bounds for array " << array << " in table " << root << std::endl;
                    return nullptr;
                }
                arr->resize(idx + 1, toml::table());
            }

            return (*arr)[idx].as_table();
        }

        if (auto node = root[path]) {
            if (!node.as_table()) {
                logger.error("-> node_for_path") << "already have " << path << " in table " << root << std::endl;
            }
            return node.as_table();
        }
        if (!create) {
            return nullptr;
        }
        auto result = root.insert(path, toml::table());
        if (!result.second) {
            logger.error("-> node_for_path")
                << "could not insert section " << path << " into table " << root << std::endl;
            return nullptr;
        }
        //return result.first->as_table();
        return root[path].as_table();
    }

    auto pre = path.substr(0, dot);
    auto parent = node_for_path(logger, root, pre, create);
    if (!parent) {
        return nullptr;
    }
    if (!parent->is_table()) {
        logger.error("-> node_for_path") << "expected table for section " << pre << " in table " << root << std::endl;
        return nullptr;
    }
    auto ptbl = parent->as_table();
    const auto post = path.substr(dot + 1);
    return node_for_path(logger, *ptbl, post, create);
}

toml::table *table_for_section(const Logger &logger, toml::table &root, const std::string &section, bool create)
{
    logger.debug("-> table_for_section") << "looking for or creating section " << section << " in table " << root
                                         << std::endl;
    auto node = node_for_path(logger, root, section, create);
    if (!node) {
        logger.debug("-> table_for_section")
            << "looked for section " << section << " in table " << root << ", not found and not created" << std::endl;
        return nullptr;
    }
    if (auto tbl = node->as_table()) {
        logger.debug("-> table_for_section")
            << "looked for section " << section << " in table " << root << ", found" << *tbl << std::endl;
        return tbl;
    }
    logger.debug("-> table_for_section") << "looked for section " << section << " in table " << root
                                         << ", found not a table" << std::endl;
    return nullptr;
}

const toml::table *table_for_section(const Logger &logger, const toml::table &root, const std::string &section)
{
    auto node = node_for_path(logger, const_cast<toml::table &>(root), section, false);
    if (!node) {
        logger.debug("-> table_for_section")
            << "looked for section " << section << " in table " << root << ", not found" << std::endl;
        return nullptr;
    }
    if (auto tbl = node->as_table()) {
        logger.debug("-> table_for_section")
            << "looked for section " << section << " in table " << root << ", found" << *tbl << std::endl;
        return tbl;
    }
    logger.debug("-> table_for_section") << "looked for section " << section << " in table " << root
                                         << ", found not a table" << std::endl;
    return nullptr;
}


template<class V>
typename Convert<V>::TomlType Convert<V>::to_toml(Entry *entry, const V &v)
{
    return v;
}

template<class V>
std::optional<typename Convert<V>::Type> Convert<V>::get_from_table(Entry *entry, const toml::table *tbl,
                                                                    const std::string &name, bool optional)
{
    if (!tbl) {
        return std::optional<Type>();
    }
    auto bracket = name.find('[');
    if (bracket != std::string::npos) {
        if (name.back() != ']') {
            entry->error("get_from_table") << "invalid name " << name << std::endl;
            return std::optional<Type>();
        }
        auto index = name.substr(bracket + 1, name.size() - bracket - 2);
        auto array = name.substr(0, bracket);
        entry->debug("get_from_table") << "name " << name << " accesses an array: array=" << array
                                       << ", index=" << std::stoul(index) << std::endl;
        auto arr = (*tbl)[array];
        if (!arr) {
            (optional ? entry->debug("get_from_table") : entry->error("get_from_table"))
                << "node for array " << array << " not found" << std::endl;
            return std::optional<Type>();
        }
        if (!arr.is_array()) {
            entry->error("get_from_table") << "node " << array << " is not an array" << std::endl;
            return std::optional<Type>();
        }
        auto a = arr.as_array();
        auto idx = std::stoul(index);
        if (idx >= a->size()) {
            (optional ? entry->debug("get_from_table") : entry->error("get_from_table"))
                << "index " << idx << " out of bounds for array " << array << std::endl;
            return std::optional<Type>();
        }
        auto &node = a->at(std::stoul(index));
        auto opt = node.template value<Type>();
        if (!opt) {
            entry->warn() << entry->key() << ": array not convertible to requested type" << std::endl;
        }
    }
    auto node = (*tbl)[name];
    if (!node) {
        entry->debug() << "node for name " << name << " not found" << std::endl;
        return std::optional<Type>();
    }
    auto opt = node.template value<Type>();
    if (!opt) {
        entry->warn() << entry->key() << ": array not convertible to requested type" << std::endl;
    }
    return opt;
}

template<class V>
std::optional<typename Convert<V>::Type> Convert<V>::as(Entry *entry, size_t index, const toml::node &n)
{
    auto v = n.template as<Type>();
    if (!v) {
        return std::optional<Type>();
    }
    return std::optional<Type>(v->get());
}

typename Convert<Section>::TomlType Convert<Section>::to_toml(Entry *entry, const typename Convert<Section>::Type &v)
{
    // FIXME
    toml::table t;
    if (v.m_tomlTable) {
        t = *static_cast<const toml::table *>(v.m_tomlTable);
    }
    return t;
}

std::optional<typename Convert<Section>::Type> Convert<Section>::get_from_table(Entry *entry, const toml::table *tbl,
                                                                                const std::string &name, bool optional)
{
    if (!tbl) {
        return std::optional<Type>();
    }

    std::string section = entry->section();
    if (!section.empty() && !name.empty())
        section += ".";
    section += name;

    auto bracket = name.find('[');
    if (bracket != std::string::npos) {
        if (name.back() != ']') {
            entry->error("get_from_table") << "invalid name " << name << std::endl;
            return std::optional<Type>();
        }
        auto index = name.substr(bracket + 1, name.size() - bracket - 2);
        auto array = name.substr(0, bracket);
        entry->debug("get_from_table") << "name " << name << " accesses an array: array=" << array
                                       << ", index=" << std::stoul(index) << std::endl;
        auto arr = (*tbl)[array];
        if (!arr) {
            (optional ? entry->debug("get_from_table") : entry->error("get_from_table"))
                << "node for array " << array << " not found" << std::endl;
            return std::optional<Type>();
        }
        if (!arr.is_array()) {
            entry->error("get_from_table") << "node " << array << " is not an array" << std::endl;
            return std::optional<Type>();
        }
        auto a = arr.as_array();
        auto idx = std::stoul(index);
        if (idx >= a->size()) {
            (optional ? entry->debug("get_from_table") : entry->error("get_from_table"))
                << "index " << idx << " out of bounds for array " << array << std::endl;
            return std::optional<Type>();
        }
        auto &node = a->at(std::stoul(index));
        auto sec = config::Section(entry->path(), section);
        sec.setTomlTable(node.as_table());
        auto opt = std::optional<Type>(sec);
        if (!opt) {
            entry->warn() << entry->key() << ": array not convertible to requested type" << std::endl;
        }
        return opt;
    }
    auto node = (*tbl)[name];
    if (!node) {
        return std::optional<Type>();
    }
    auto sec = config::Section(entry->path(), section);
    sec.setTomlTable(node.as_table());
    auto opt = std::optional<Type>(sec);
    return opt;
}

const std::optional<typename Convert<Section>::Type> Convert<Section>::as(Entry *entry, size_t index,
                                                                          const toml::node &n)
{
    auto section = entry->section();
    auto name = entry->name();
    if (!section.empty() && !name.empty())
        section += ".";
    section += name;
    section += "[" + std::to_string(index) + "]";
    auto tbl = n.as_table();
    if (!tbl) {
        entry->warn() << entry->key() << ": expected table for section" << std::endl;
        return std::optional<config::Section>();
    }
    config::Section sec(entry->path(), section);
    sec.setTomlTable(tbl);
    return std::optional<config::Section>(sec);
}

template struct Convert<bool>;
template struct Convert<int64_t>;
template struct Convert<double>;
template struct Convert<std::string>;

} // namespace detail
} // namespace config

#ifdef CONFIG_NAMESPACE
}
#endif
