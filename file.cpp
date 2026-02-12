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

} // namespace


File::File(const std::string &path, detail::Manager *mgr)
: Section(path, "", mgr ? mgr : detail::Manager::the())
{}

File::~File() = default;

bool File::exists() const
{
    return m_config->exists;
}

std::string File::pathname() const
{
    return m_config->base + sep + m_config->path + ".toml";
}

bool File::save()
{
    return m_manager->save(m_config->path);
}

void File::setSaveOnExit(bool enable)
{
    m_config->autosave = enable;
}

bool File::isSaveOnExit() const
{
    return m_config->autosave;
}

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
