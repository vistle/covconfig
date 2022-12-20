// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file config.h
/// convenience header for accessing the complete library interface
#pragma once

#ifdef CONFIG_NAMESPACE
/// optional toplevel namespace
/** #define CONFIG_NAMESPACE in order to build an independent instance of the configuration
    library where all entities are defined within the toplevel namespace CONFIG_NAMESPACE
 */

namespace CONFIG_NAMESPACE {
#endif

/// namespace for CovConfig API
namespace config {

/// internal CovConfig namespace
namespace detail {}
} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif

#include "access.h"
#include "value.h"
#include "array.h"
