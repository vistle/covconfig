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
#define COV_IMPORT V_EXPORT
#else
#define COV_IMPORT
#define COV_EXPORT
#endif

#if defined(COVCONFIG_VISTLE)
#if defined(vistle_covconfig_EXPORTS)
#define COVEXPORT COV_EXPORT
#else
#define COVEXPORT COV_IMPORT
#endif
#elif defined(COVCONFIG_OPENCOVER)
#if defined(coOpenCovConfig_EXPORTS)
#define COVEXPORT COV_EXPORT
#else
#define COVEXPORT COV_IMPORT
#endif
#elif defined(COVCONFIG_COVISE)
#if defined(coCovConfig_EXPORTS)
#define COVEXPORT COV_EXPORT
#else
#define COVEXPORT COV_IMPORT
#endif
#else
#define COVEXPORT
#endif
