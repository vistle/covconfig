#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include <string>
#include "export.h"

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

class V_CONFIGEXPORT Accessor {
    Accessor();
    Accessor(const std::string &cluster, int rank);
    void setPrefix(const std::string &dir);
    virtual ~Accessor();
};

bool V_CONFIGEXPORT
init(const std::string &cluster = std::string(),
     int rank = -1); //< initiate access to config system and configure look-up for cluster/rank specific entries
bool isConfigured(); //< whether config system is ready for use
bool V_CONFIGEXPORT acquire(); //< initiate access to config system
bool V_CONFIGEXPORT release(); //< revoke access to config system
void V_CONFIGEXPORT setPrefix(const std::string &dir); //< add software installation as additional source directory
}
#ifdef CONFIG_NAMESPACE
}
#endif
#endif
