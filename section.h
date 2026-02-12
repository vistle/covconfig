// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file section.h
/// query available settings within a section
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iosfwd>
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
class Section;
class File;
class ConfigBase;

COVEXPORT std::ostream& operator<<(std::ostream& os, const Section& section);
COVEXPORT std::ostream& operator<<(std::ostream& os, const std::vector<Section> &section);
COVEXPORT bool operator==(const Section& lhs, const Section& rhs);
COVEXPORT bool operator!=(const Section& lhs, const Section& rhs);

/// query for existing sections and entries within a configuration section
class COVEXPORT Section: protected detail::Logger {
    friend bool operator==(const Section& lhs, const Section& rhs);
    friend bool operator!=(const Section& lhs, const Section& rhs);
    friend std::ostream& operator<<(std::ostream& os, const Section& section);
    friend class File;

public:
    Section(detail::Manager *mgr = nullptr); ///< create an interface to a configuration file
    Section(Section *parent, const std::string &name); ///< create an interface to a subsection
    Section(const std::string &path, const std::string &section, detail::Manager *mgr = nullptr); ///< create an interface to a configuration file
    ~Section(); ///< destructor

    std::string sectionname() const;

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

    void setTomlTable(const void *tbl);

private:
    std::string m_section;
    detail::Manager *m_manager = nullptr;
    std::shared_ptr<detail::Config> m_config;
    const void *m_tomlTable = nullptr;
};

extern template std::unique_ptr<Value<bool>> COVEXPORT Section::value<bool>(const std::string &section,
                                                                         const std::string &name);
extern template std::unique_ptr<Value<int64_t>> COVEXPORT Section::value<int64_t>(const std::string &section,
                                                                               const std::string &name);
extern template std::unique_ptr<Value<double>> COVEXPORT Section::value<double>(const std::string &section,
                                                                             const std::string &name);
extern template std::unique_ptr<Value<std::string>> COVEXPORT Section::value<std::string>(const std::string &section,
                                                                                       const std::string &name);
extern template std::unique_ptr<Value<bool>> COVEXPORT Section::value(const std::string &section, const std::string &name,
                                                                   const bool &def, Flag flags);
extern template std::unique_ptr<Value<int64_t>>
    COVEXPORT Section::value(const std::string &section, const std::string &name, const int64_t &def, Flag flags);
extern template std::unique_ptr<Value<double>>
    COVEXPORT Section::value(const std::string &section, const std::string &name, const double &def, Flag flags);
extern template std::unique_ptr<Value<std::string>>
    COVEXPORT Section::value(const std::string &section, const std::string &name, const std::string &def, Flag flags);

extern template std::unique_ptr<Array<bool>> COVEXPORT Section::array(const std::string &section, const std::string &name);
extern template std::unique_ptr<Array<int64_t>> COVEXPORT Section::array(const std::string &section,
                                                                      const std::string &name);
extern template std::unique_ptr<Array<double>> COVEXPORT Section::array(const std::string &section,
                                                                     const std::string &name);
extern template std::unique_ptr<Array<std::string>> COVEXPORT Section::array(const std::string &section,
                                                                          const std::string &name);
extern template std::unique_ptr<Array<bool>> COVEXPORT Section::array(const std::string &section, const std::string &name,
                                                                   const std::vector<bool> &def, Flag flags);
extern template std::unique_ptr<Array<int64_t>> COVEXPORT Section::array(const std::string &section,
                                                                      const std::string &name,
                                                                      const std::vector<int64_t> &def, Flag flags);
extern template std::unique_ptr<Array<double>> COVEXPORT Section::array(const std::string &section,
                                                                     const std::string &name,
                                                                     const std::vector<double> &def, Flag flags);
extern template std::unique_ptr<Array<std::string>> COVEXPORT Section::array(const std::string &section,
                                                                          const std::string &name,
                                                                          const std::vector<std::string> &def,
                                                                          Flag flags);

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
