#ifndef CONFIG_EXPORT_H
#define CONFIG_EXPORT_H

#ifdef CONFIG_NAMESPACE
#if CONFIG_NAMESPACE == vistle
#include <vistle/util/export.h>

#if defined(vistle_config_EXPORTS)
#define V_CONFIGEXPORT V_EXPORT
#else
#define V_CONFIGEXPORT V_IMPORT
#endif
#endif
#else
#define V_CONFIGEXPORT
#endif

#endif
