// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "base.h"
#include "entry.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

ConfigBase::~ConfigBase() = default;

bool ConfigBase::exists() const
{
    return m_entry->exists();
}

bool ConfigBase::hasDefaultValue() const
{
    return m_entry->hasDefaultValue();
}

const std::string &ConfigBase::path() const
{
    return m_entry->path();
}

const std::string &ConfigBase::section() const
{
    return m_entry->section();
}

const std::string &ConfigBase::name() const
{
    return m_entry->name();
}

std::string ConfigBase::key() const
{
    return m_entry->key();
}

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
