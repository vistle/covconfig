// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file output.h
/// output utilities
#pragma once

#include "export.h"
#include <ostream>
#include <vector>
#include <string>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {

template<typename V>
std::ostream &operator<<(std::ostream &os, const std::vector<V> &vec);

extern template std::ostream &operator<<(std::ostream &os, const std::vector<bool> &vec);
extern template std::ostream &operator<<(std::ostream &os, const std::vector<char> &vec);
extern template std::ostream &operator<<(std::ostream &os, const std::vector<int64_t> &vec);
extern template std::ostream &operator<<(std::ostream &os, const std::vector<double> &vec);
extern template std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &vec);

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
