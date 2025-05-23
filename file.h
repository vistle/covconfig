// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file file.h
/// query available settings within a file
#pragma once

#include <string>
#include <memory>
#include <vector>
#include "detail/export.h"
#include "detail/flags.h"
#include "detail/logger.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {
class Manager;
struct Config;
} // namespace detail
class ConfigBase;
template<class V>
class Value;
template<class V>
class Array;

//enable value funcions only for supported non-array types
template<class V>
using ValuePtr = std::enable_if_t<detail::isValueType<V>::value,
                                  std::unique_ptr<Value<V>>>; ///< unique pointer to a configuration value

/// query for existing sections and entries within a configuration file
class COVEXPORT File: detail::Logger {
public:
    File(const std::string &path, detail::Manager *mgr = nullptr); ///< create an interface to a configuration file
    ~File(); ///< destructor

    bool exists() const; ///< query if path has existed when configuration was loaded
    std::string pathname() const; ///< actual path that was/would have been loaded
    bool save(); ///< request to store current configuration to disk
    void setSaveOnExit(
        bool enable); ///< request to save the current values when Manager is destroyed (i.e. application quits)
    bool isSaveOnExit() const; ///< query whether file will be saved automatically on exit

    std::vector<std::string> sections(); ///< all top-level sections
    std::vector<std::string> subsections(const std::string &section); ///< all subsections of a (sub-)section
    std::vector<std::string> entries(const std::string &section); ///< all entries within a (sub-)section

    template<class V>
    ValuePtr<V> value(const std::string &section,
                      const std::string &name); ///< query existing configuration value
    template<class V>
    ValuePtr<V> value(const std::string &section, const std::string &name, const V &def,
                      Flag flags = Flag::Default); ///< create configuration value with the provided default

    template<class V>
    std::unique_ptr<Array<V>> array(const std::string &section,
                                    const std::string &name); ///< query existing configuration array
    template<class V>
    std::unique_ptr<Array<V>>
    array(const std::string &section, const std::string &name, const std::vector<V> &def,
          Flag flags = Flag::Default); ///< create configuration array with the provided default

private:
    detail::Manager *m_manager = nullptr;
    detail::Config &m_config;
};

extern template std::unique_ptr<Value<bool>> COVEXPORT File::value<bool>(const std::string &section,
                                                                         const std::string &name);
extern template std::unique_ptr<Value<int64_t>> COVEXPORT File::value<int64_t>(const std::string &section,
                                                                               const std::string &name);
extern template std::unique_ptr<Value<double>> COVEXPORT File::value<double>(const std::string &section,
                                                                             const std::string &name);
extern template std::unique_ptr<Value<std::string>> COVEXPORT File::value<std::string>(const std::string &section,
                                                                                       const std::string &name);
extern template std::unique_ptr<Value<bool>> COVEXPORT File::value(const std::string &section, const std::string &name,
                                                                   const bool &def, Flag flags);
extern template std::unique_ptr<Value<int64_t>>
    COVEXPORT File::value(const std::string &section, const std::string &name, const int64_t &def, Flag flags);
extern template std::unique_ptr<Value<double>>
    COVEXPORT File::value(const std::string &section, const std::string &name, const double &def, Flag flags);
extern template std::unique_ptr<Value<std::string>>
    COVEXPORT File::value(const std::string &section, const std::string &name, const std::string &def, Flag flags);

extern template std::unique_ptr<Array<bool>> COVEXPORT File::array(const std::string &section, const std::string &name);
extern template std::unique_ptr<Array<int64_t>> COVEXPORT File::array(const std::string &section,
                                                                      const std::string &name);
extern template std::unique_ptr<Array<double>> COVEXPORT File::array(const std::string &section,
                                                                     const std::string &name);
extern template std::unique_ptr<Array<std::string>> COVEXPORT File::array(const std::string &section,
                                                                          const std::string &name);
extern template std::unique_ptr<Array<bool>> COVEXPORT File::array(const std::string &section, const std::string &name,
                                                                   const std::vector<bool> &def, Flag flags);
extern template std::unique_ptr<Array<int64_t>> COVEXPORT File::array(const std::string &section,
                                                                      const std::string &name,
                                                                      const std::vector<int64_t> &def, Flag flags);
extern template std::unique_ptr<Array<double>> COVEXPORT File::array(const std::string &section,
                                                                     const std::string &name,
                                                                     const std::vector<double> &def, Flag flags);
extern template std::unique_ptr<Array<std::string>> COVEXPORT File::array(const std::string &section,
                                                                          const std::string &name,
                                                                          const std::vector<std::string> &def,
                                                                          Flag flags);

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
