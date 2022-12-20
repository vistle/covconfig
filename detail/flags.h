// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file flag.h
/// setting flags enum
#pragma once

#include <string>
#include "export.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

enum class COVEXPORT Flag {
    Default = 0,
    PerModel = 1,
};
}
#ifdef CONFIG_NAMESPACE
}
#endif
