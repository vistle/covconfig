// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file access.h
/// organize access to configuration system
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "detail/export.h"
#include "detail/flags.h"
#include "detail/logger.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {
class Manager;
}
class File;
class ConfigBase;
template<class V>
class Value;
template<class V>
class Array;

/// provide a bridge for retrieving and storing per-model configuration values
/** When configuration entries tagged with Flag::PerModel are changed, the registered bridge is notified of this via \ref Bridge::wasChanged.
    It is the Bridge's responsibility to store the changed entry as required.
    If the bridge needs to change configuration entries, then it can do so by modifying them with the interfaces provided by \ref Array or \ref Value.
 */
class COVEXPORT Bridge {
public:
    virtual ~Bridge();
    virtual bool wasChanged(
        const ConfigBase
            *entry) = 0; ///< called to notify that a configuration entry (\ref Value or \ref Array) has been changed
};

//enable value functions only for supported non-array types
template<class V>
using ValuePtr = std::enable_if_t<detail::isValueType<V>::value,
                                  std::unique_ptr<Value<V>>>; ///< unique pointer to a configuration value

/// organize access to configuration system
/** Creating an instance of Access controls access to the configuration system via a Manager. This Manager is created and destroyed as needed. */
class COVEXPORT Access: detail::Logger {
public:
    static bool isInitialized(); ///< check if Access has already been initialized with the non-default constructor

    Access(); ///< initiate access to configuration system with default search path only
    Access(const std::string &host, const std::string &cluster,
           int rank = -1); ///< initiate access to configuration, preferring configuration files for host and cluster
    virtual ~Access(); ///< revoke access to configuration system
    const std::string &cluster() const; ///< query configured cluster for which configuration is read
    const std::string &hostname() const; ///< query configured hostname for which configuration is read
    void setPrefix(const std::string &dir); ///< set software installation prefix as additional search path
    bool setWorkspaceBridge(Bridge *bridge); ///< specify \ref Bridge for accessing per-model configuration values
    bool removeWorkspaceBridge(Bridge *bridge); ///< remove per-model configuration \ref Bridge
    void
    setErrorHandler(std::function<void()> handler = nullptr); ///< what to do in case of errors, initially calls exit
    bool save(); ///< save changes in all files that should be saved on exit

    std::unique_ptr<File> file(const std::string &path) const; ///< get interface to a configuration file

    template<class V>
    ValuePtr<V> value(const std::string &path, const std::string &section,
                      const std::string &name); ///< query existing configuration value
    template<class V>
    ValuePtr<V> value(const std::string &path, const std::string &section, const std::string &name, const V &def,
                      Flag flags = Flag::Default); ///< create configuration value with the provided default

    template<class V>
    std::unique_ptr<Array<V>> array(const std::string &path, const std::string &section,
                                    const std::string &name); ///< query existing configuration array
    template<class V>
    std::unique_ptr<Array<V>>
    array(const std::string &path, const std::string &section, const std::string &name, const std::vector<V> &def,
          Flag flags = Flag::Default); ///< create configuration array with the provided default

private:
    Bridge *m_bridge = nullptr;
    detail::Manager *m_manager = nullptr;
};

extern template std::unique_ptr<Value<bool>>
    COVEXPORT Access::value<bool>(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Value<int64_t>>
    COVEXPORT Access::value<int64_t>(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Value<double>>
    COVEXPORT Access::value<double>(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Value<std::string>>
    COVEXPORT Access::value<std::string>(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Value<bool>> COVEXPORT Access::value(const std::string &path,
                                                                     const std::string &section,
                                                                     const std::string &name, const bool &def,
                                                                     Flag flags);
extern template std::unique_ptr<Value<int64_t>> COVEXPORT Access::value(const std::string &path,
                                                                        const std::string &section,
                                                                        const std::string &name, const int64_t &def,
                                                                        Flag flags);
extern template std::unique_ptr<Value<double>> COVEXPORT Access::value(const std::string &path,
                                                                       const std::string &section,
                                                                       const std::string &name, const double &def,
                                                                       Flag flags);
extern template std::unique_ptr<Value<std::string>> COVEXPORT Access::value(const std::string &path,
                                                                            const std::string &section,
                                                                            const std::string &name,
                                                                            const std::string &def, Flag flags);

extern template std::unique_ptr<Array<bool>>
    COVEXPORT Access::array(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Array<int64_t>>
    COVEXPORT Access::array(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Array<double>>
    COVEXPORT Access::array(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Array<std::string>>
    COVEXPORT Access::array(const std::string &path, const std::string &section, const std::string &name);
extern template std::unique_ptr<Array<bool>> COVEXPORT Access::array(const std::string &path,
                                                                     const std::string &section,
                                                                     const std::string &name,
                                                                     const std::vector<bool> &def, Flag flags);
extern template std::unique_ptr<Array<int64_t>> COVEXPORT Access::array(const std::string &path,
                                                                        const std::string &section,
                                                                        const std::string &name,
                                                                        const std::vector<int64_t> &def, Flag flags);
extern template std::unique_ptr<Array<double>> COVEXPORT Access::array(const std::string &path,
                                                                       const std::string &section,
                                                                       const std::string &name,
                                                                       const std::vector<double> &def, Flag flags);
extern template std::unique_ptr<Array<std::string>>
    COVEXPORT Access::array(const std::string &path, const std::string &section, const std::string &name,
                            const std::vector<std::string> &def, Flag flags);
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
