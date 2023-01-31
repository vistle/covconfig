// Copyright (C) High-Performance Computing Center Stuttgart (https://www.hlrs.de/)
// SPDX-License-Identifier: LGPL-2.1-or-later

/// \file export.h
/// control symbol visibility
#pragma once

#if defined(_WIN32) && !defined(NODLL)
#define COV_IMPORT __declspec(dllimport)
#define COV_EXPORT __declspec(dllexport)

#elif defined(__GNUC__) && __GNUC__ >= 4
#define COV_EXPORT __attribute__((visibility("default")))
#define COV_IMPORT COV_EXPORT
#else
#define COV_IMPORT
#define COV_EXPORT
#endif


#if defined(vistle_config_EXPORTS)
#define COVEXPORT COV_EXPORT
#elif defined(coOpenConfig_EXPORTS)
#define COVEXPORT COV_EXPORT
#elif defined(coConfigNew_EXPORTS)
#define COVEXPORT COV_EXPORT
#else
#define COVEXPORT COV_IMPORT
#endif
