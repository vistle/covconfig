// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

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

const std::string sep("/");

const toml::table *table_for_section(const toml::table &root, const std::string &section)
{
    if (section.empty())
        return &root;
    return root[section].as_table();
    return nullptr;
}

} // namespace


File::File(const std::string &path, detail::Manager *mgr)
: Logger("File"), m_manager(mgr ? mgr : Manager::the()), m_config(m_manager->registerPath(path))
{}

File::~File() = default;

bool File::exists() const
{
    return m_config.exists;
}

const std::string File::pathname() const
{
    return m_config.base + sep + m_config.path + sep + ".toml";
}

bool File::save()
{
    return m_manager->save(m_config.path);
}

void File::enableSaveOnExit(bool enable)
{
    m_config.autosave = enable;
}

bool File::isSaveOnExit() const
{
    return m_config.autosave;
}

std::vector<std::string> File::sections()
{
    return subsections("");
}

std::vector<std::string> File::subsections(const std::string &section)
{
    const std::string prefix = section.empty() ? "" : section + ".";
    std::vector<std::string> sections;
    if (const auto *tbl = table_for_section(m_config.config, section)) {
        for (auto it = tbl->begin(); it != tbl->end(); ++it) {
            if (it->second.is_table()) {
                sections.emplace_back(prefix + std::string(it->first.str()));
            }
        }
    }
    return sections;
}

std::vector<std::string> File::entries(const std::string &section)
{
    std::vector<std::string> entries;
    if (const auto *tbl = table_for_section(m_config.config, section)) {
        for (auto it = tbl->begin(); it != tbl->end(); ++it) {
            if (!it->second.is_table())
                entries.emplace_back(it->first.str());
        }
    }
    return entries;
}

template<class V>
std::unique_ptr<Value<V>> File::value(const std::string &section, const std::string &name)
{
    const auto &path = m_config.path;
    return std::make_unique<Value<V>>(path, section, name, m_manager);
}

template<class V>
std::unique_ptr<Value<V>> File::value(const std::string &section, const std::string &name, const V &def, Flag flags)
{
    const auto &path = m_config.path;
    return std::make_unique<Value<V>>(path, section, name, def, m_manager, flags);
}

template<class V>
std::unique_ptr<Array<V>> File::array(const std::string &section, const std::string &name)
{
    const auto &path = m_config.path;
    return std::make_unique<Array<V>>(path, section, name, m_manager);
}

template<class V>
std::unique_ptr<Array<V>> File::array(const std::string &section, const std::string &name, const std::vector<V> &def,
                                      Flag flags)
{
    const auto &path = m_config.path;
    return std::make_unique<Array<V>>(path, section, name, def, m_manager, flags);
}


template std::unique_ptr<Value<bool>> COVEXPORT File::value(const std::string &section, const std::string &name);
template std::unique_ptr<Value<int64_t>> COVEXPORT File::value(const std::string &section, const std::string &name);
template std::unique_ptr<Value<double>> COVEXPORT File::value(const std::string &section, const std::string &name);
template std::unique_ptr<Value<std::string>> COVEXPORT File::value(const std::string &section, const std::string &name);
template std::unique_ptr<Value<bool>> COVEXPORT File::value(const std::string &section, const std::string &name,
                                                            const bool &def, Flag flags);
template std::unique_ptr<Value<int64_t>> COVEXPORT File::value(const std::string &section, const std::string &name,
                                                               const int64_t &def, Flag flags);
template std::unique_ptr<Value<double>> COVEXPORT File::value(const std::string &section, const std::string &name,
                                                              const double &def, Flag flags);
template std::unique_ptr<Value<std::string>> COVEXPORT File::value(const std::string &section, const std::string &name,
                                                                   const std::string &def, Flag flags);

template std::unique_ptr<Array<bool>> COVEXPORT File::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<int64_t>> COVEXPORT File::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<double>> COVEXPORT File::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<std::string>> COVEXPORT File::array(const std::string &section, const std::string &name);
template std::unique_ptr<Array<bool>> COVEXPORT File::array(const std::string &section, const std::string &name,
                                                            const std::vector<bool> &def, Flag flags);
template std::unique_ptr<Array<int64_t>> COVEXPORT File::array(const std::string &section, const std::string &name,
                                                               const std::vector<int64_t> &def, Flag flags);
template std::unique_ptr<Array<double>> COVEXPORT File::array(const std::string &section, const std::string &name,
                                                              const std::vector<double> &def, Flag flags);
template std::unique_ptr<Array<std::string>> COVEXPORT File::array(const std::string &section, const std::string &name,
                                                                   const std::vector<std::string> &def, Flag flags);

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
