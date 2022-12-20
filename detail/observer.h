// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

/// \file observer.h
/// observe configuration data changes
#include "export.h"
#include <cstdlib>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {
namespace detail {

class Observer {
public:
    virtual void update() = 0;
};

} // namespace detail
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
