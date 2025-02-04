// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "access.h"
#include "value.h"
#include "array.h"
#include "file.h"
#include "detail/manager.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
using namespace detail;

Bridge::~Bridge() = default;

bool Access::isInitialized()
{
    return Manager::exists();
}

Access::Access(): Logger("Access")
{
    m_manager = detail::Manager::the();
    m_manager->acquire();
}

Access::Access(const std::string &host, const std::string &cluster, int rank): Logger("Access")
{
    if (Manager::exists()) {
        assert(Manager::the());
        m_manager = Manager::the();
        if (host != m_manager->hostname() || cluster != m_manager->cluster() || rank != m_manager->rank()) {
            if (rank == -1) {
                debug() << "not changing rank " << m_manager->rank() << " to " << rank << std::endl;
            } else if (m_manager->rank() == -1) {
                m_manager->setRank(rank);
            } else {
                error() << "cannot configure for host=" << host << ", cluster=" << cluster << ":" << rank
                        << ", already configured for " << m_manager->hostname() << " in " << m_manager->cluster() << ":"
                        << m_manager->rank() << std::endl;
                assert(m_manager->hostname() == host);
                assert(m_manager->cluster() == cluster);
                assert(m_manager->rank() == rank);
                m_manager->handleError();
            }
        }
    } else {
        m_manager = new Manager(host, cluster, rank);
        assert(Manager::the());
    }
    m_manager->acquire();
}

const std::string &Access::hostname() const
{
    assert(m_manager);
    return m_manager->hostname();
}

const std::string &Access::cluster() const
{
    assert(m_manager);
    return m_manager->cluster();
}

void Access::setPrefix(const std::string &dir)
{
    assert(m_manager);
    m_manager->setPrefix(dir);
}

void Access::setErrorHandler(std::function<void()> handler)
{}

bool Access::setWorkspaceBridge(Bridge *bridge)
{
    assert(m_manager);
    if (m_bridge)
        return false;
    m_bridge = bridge;
    m_manager->setWorkspaceBridge(m_bridge);
    return true;
}

bool Access::removeWorkspaceBridge(Bridge *bridge)
{
    assert(m_manager);
    if (m_bridge != bridge)
        return false;
    m_manager->removeWorkspaceBridge(m_bridge);
    m_bridge = nullptr;
    return true;
}

bool Access::save()
{
    if (!m_manager) {
        return false;
    }

    return m_manager->saveAllAutosave();
}

Access::~Access()
{
    if (m_manager) {
        removeWorkspaceBridge(m_bridge);
        m_manager->release();
    }
    m_manager = nullptr;
}

std::unique_ptr<File> Access::file(const std::string &path) const
{
    return std::make_unique<File>(path, m_manager);
}

template<class V>
ValuePtr<V> Access::value(const std::string &path, const std::string &section, const std::string &name)
{
    return std::make_unique<Value<V>>(path, section, name, m_manager);
}

template<class V>
ValuePtr<V> Access::value(const std::string &path, const std::string &section, const std::string &name, const V &def,
                          Flag flags)
{
    return std::make_unique<Value<V>>(path, section, name, def, m_manager, flags);
}

template<class V>
std::unique_ptr<Array<V>> Access::array(const std::string &path, const std::string &section, const std::string &name)
{
    return std::make_unique<Array<V>>(path, section, name, m_manager);
}

template<class V>
std::unique_ptr<Array<V>> Access::array(const std::string &path, const std::string &section, const std::string &name,
                                        const std::vector<V> &def, Flag flags)
{
    return std::make_unique<Array<V>>(path, section, name, def, m_manager, flags);
}

template std::unique_ptr<Value<bool>> Access::value<bool>(const std::string &path, const std::string &section,
                                                          const std::string &name);
template std::unique_ptr<Value<int64_t>> Access::value<int64_t>(const std::string &path, const std::string &section,
                                                                const std::string &name);
template std::unique_ptr<Value<double>> Access::value<double>(const std::string &path, const std::string &section,
                                                              const std::string &name);
template std::unique_ptr<Value<std::string>>
Access::value<std::string>(const std::string &path, const std::string &section, const std::string &name);

template std::unique_ptr<Value<bool>> Access::value(const std::string &path, const std::string &section,
                                                    const std::string &name, const bool &def, Flag flags);
template std::unique_ptr<Value<int64_t>> Access::value(const std::string &path, const std::string &section,
                                                       const std::string &name, const int64_t &def, Flag flags);
template std::unique_ptr<Value<double>> Access::value(const std::string &path, const std::string &section,
                                                      const std::string &name, const double &def, Flag flags);
template std::unique_ptr<Value<std::string>> Access::value(const std::string &path, const std::string &section,
                                                           const std::string &name, const std::string &def, Flag flags);

template std::unique_ptr<Array<bool>> Access::array(const std::string &path, const std::string &section,
                                                    const std::string &name);
template std::unique_ptr<Array<int64_t>> Access::array(const std::string &path, const std::string &section,
                                                       const std::string &name);
template std::unique_ptr<Array<double>> Access::array(const std::string &path, const std::string &section,
                                                      const std::string &name);
template std::unique_ptr<Array<std::string>> Access::array(const std::string &path, const std::string &section,
                                                           const std::string &name);

template std::unique_ptr<Array<bool>> Access::array(const std::string &path, const std::string &section,
                                                    const std::string &name, const std::vector<bool> &def, Flag flags);
template std::unique_ptr<Array<int64_t>> Access::array(const std::string &path, const std::string &section,
                                                       const std::string &name, const std::vector<int64_t> &def,
                                                       Flag flags);
template std::unique_ptr<Array<double>> Access::array(const std::string &path, const std::string &section,
                                                      const std::string &name, const std::vector<double> &def,
                                                      Flag flags);
template std::unique_ptr<Array<std::string>> Access::array(const std::string &path, const std::string &section,
                                                           const std::string &name, const std::vector<std::string> &def,
                                                           Flag flags);
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
