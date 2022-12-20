// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "output.h"
#include <string>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

namespace detail {

template<typename V>
std::ostream &operator<<(std::ostream &os, const std::vector<V> &vec)
{
    os << "{";
    for (const auto &v: vec)
        os << " " << v;
    os << " }";
    return os;
}

template std::ostream &operator<<(std::ostream &os, const std::vector<bool> &vec);
template std::ostream &operator<<(std::ostream &os, const std::vector<char> &vec);
template std::ostream &operator<<(std::ostream &os, const std::vector<int64_t> &vec);
template std::ostream &operator<<(std::ostream &os, const std::vector<double> &vec);
template std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &vec);

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
