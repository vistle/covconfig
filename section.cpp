// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "section.h"
#include "file.h"
#include "value.h"
#include "array.h"
#include "detail/manager.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include "detail/toml/toml.hpp"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
using namespace detail;

namespace {
    std::string section_prefix(const std::string &root, const std::string &section = std::string(), const std::string &subsection = std::string()) {

        std::string p = root;
        if (!section.empty()) {
            if (!p.empty())
                p += ".";
            p += section;
        }
        if (!subsection.empty()) {
            if (!p.empty())
                p += ".";
            p += subsection;
        }
        return p;
    }
}

Section::Section(detail::Manager *mgr)
: Logger("Section"), m_manager(mgr ? mgr : detail::Manager::the())
{
   debug() << "default created" << std::endl;
}

Section::Section(Section *parent, const std::string &name)
: Logger("Section"), m_manager(parent->m_manager), m_config(parent->m_config)
{
    m_section = section_prefix(parent->sectionname(), name);
    debug() << "created from parent " << parent->sectionname() << " with section=" << m_section << std::endl;
}

Section::Section(const std::string &path, const std::string &section, detail::Manager *mgr)
: Logger("Section"), m_section(section), m_manager(mgr ? mgr : detail::Manager::the()), m_config(m_manager->registerPath(path))
{
    debug() << "created with section=" << m_section << std::endl;
}

Section::~Section() = default;

void Section::setTomlTable(const void *tbl)
{
    m_tomlTable = tbl;
}

std::string Section::sectionname() const
{
    return m_section;
}

std::vector<std::string> Section::sections()
{
    return subsections("");
}

std::vector<std::string> Section::subsections(const std::string &section)
{
    std::vector<std::string> sections;
    if (!m_config)
        return sections;

    std::string prefix = section_prefix(m_section);
    if (!prefix.empty())
        prefix += ".";

    if (const auto *tbl = static_cast<const toml::table *>(m_tomlTable)) {
        for (auto it = tbl->begin(); it != tbl->end(); ++it) {
            if (it->second.is_table()) {
                sections.emplace_back(prefix + std::string(it->first.str()));
            }
        }
    }

    return sections;
}

std::vector<std::string> Section::entries(const std::string &section)
{
    std::vector<std::string> entries;
    if (const auto *tbl = static_cast<const toml::table *>(m_tomlTable)) {
        debug("Section::entries") << "entries for section " + section + ":" << std::endl;
        for (auto it = tbl->begin(); it != tbl->end(); ++it) {
            debug("Section::entries") << "   " + std::string(it->first.str()) << std::endl;
            if (!it->second.is_table())
                entries.emplace_back(it->first.str());
        }
    } else {
        debug("no entries for section " + section);
    }
    return entries;
}

template<class V>
ValuePtr<V> Section::value(const std::string &section, const std::string &name)
{
    if (!m_config)
        return std::make_unique<Value<V>>("", m_section, name, m_manager);

    auto prefix = section_prefix(m_section, section);
    return std::make_unique<Value<V>>(m_config->path, prefix, name, m_manager);
}

template<class V>
ValuePtr<V> Section::value(const std::string &section, const std::string &name, const V &def, Flag flags)
{
    if (!m_config)
        return std::make_unique<Value<V>>("", m_section, name, m_manager);

    auto prefix = section_prefix(m_section, section);
    return std::make_unique<Value<V>>(m_config->path, prefix, name, def, m_manager, flags);
}

template<class V>
std::unique_ptr<Array<V>> Section::array(const std::string &section, const std::string &name)
{
    if (!m_config)
        return std::make_unique<Array<V>>("", m_section, name, m_manager);

    auto prefix = section_prefix(m_section, section);
    return std::make_unique<Array<V>>(m_config->path, prefix, name, m_manager);
}

template<class V>
std::unique_ptr<Array<V>> Section::array(const std::string &section, const std::string &name, const std::vector<V> &def,
                                      Flag flags)
{
    if (!m_config)
        return std::make_unique<Array<V>>("", m_section, name, m_manager);

    auto prefix = section_prefix(m_section, section);
    return std::make_unique<Array<V>>(m_config->path, prefix, name, def, m_manager, flags);
}

std::ostream& operator<<(std::ostream& os, const Section& section)
{
    os << "section: " << section.sectionname();
    if (section.m_config) {
        auto conf = section.m_config->config;
        auto sec = section.m_section;
        auto tbl = static_cast<const toml::table *>(section.m_tomlTable);
        if (tbl)
            os << ", " << *tbl << std::endl;
    } else {
        os << ", (nil config)";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Section> &sections)
{
    for (const auto &section : sections) {
        os << section << std::endl;
    }
    return os;
}

bool operator==(const Section& lhs, const Section& rhs)
{
    return lhs.m_config == rhs.m_config && lhs.m_section == rhs.m_section;
}

bool operator!=(const Section& lhs, const Section& rhs)
{
    return !(lhs == rhs);
}


template std::unique_ptr<Value<bool>> COVEXPORT Section::value<bool>(const std::string &section, const std::string &name);
template std::unique_ptr<Value<int64_t>> COVEXPORT Section::value<int64_t>(const std::string &section,
                                                                        const std::string &name);
template std::unique_ptr<Value<double>> COVEXPORT Section::value<double>(const std::string &section,
                                                                      const std::string &name);
template std::unique_ptr<Value<std::string>> COVEXPORT Section::value<std::string>(const std::string &section,
                                                                                const std::string &name);
template std::unique_ptr<Value<bool>> COVEXPORT Section::value(const std::string &section, const std::string &name,
                                                            const bool &def, Flag flags);
template std::unique_ptr<Value<int64_t>> COVEXPORT Section::value(const std::string &section, const std::string &name,
                                                               const int64_t &def, Flag flags);
template std::unique_ptr<Value<double>> COVEXPORT Section::value(const std::string &section, const std::string &name,
                                                              const double &def, Flag flags);
template std::unique_ptr<Value<std::string>> COVEXPORT Section::value(const std::string &section, const std::string &name,
                                                                   const std::string &def, Flag flags);

template std::unique_ptr<Array<bool>> COVEXPORT Section::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<int64_t>> COVEXPORT Section::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<double>> COVEXPORT Section::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<std::string>> COVEXPORT Section::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<bool>> COVEXPORT Section::array(const std::string &section, const std::string &name,
                                                            const std::vector<bool> &def, Flag flags);
template std::unique_ptr<Array<int64_t>> COVEXPORT Section::array(const std::string &section, const std::string &name,
                                                               const std::vector<int64_t> &def, Flag flags);
template std::unique_ptr<Array<double>> COVEXPORT Section::array(const std::string &section, const std::string &name,
                                                              const std::vector<double> &def, Flag flags);
template std::unique_ptr<Array<std::string>> COVEXPORT Section::array(const std::string &section, const std::string &name,
                                                                   const std::vector<std::string> &def, Flag flags);

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
