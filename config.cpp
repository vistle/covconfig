#include "config.h"
#include "manager.h"
#include <iostream>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

bool init(const std::string &cluster, int rank)
{
    if (Manager::exists()) {
        assert(Manager::the());
        const auto &manager = *Manager::the();
        if (cluster != manager.cluster() || rank != manager.rank()) {
            std::cerr << "config::init: cannot configure for " << cluster << ":" << rank << ", already configured for "
                      << manager.cluster() << ":" << manager.rank() << std::endl;
            assert(manager.cluster() == cluster);
            assert(manager.rank() == rank);
            return false;
        }
    } else {
        new Manager(cluster, rank);
        assert(Manager::the());
    }
    Manager::the()->acquire();
    return true;
}

bool acquire()
{
    if (!Manager::exists())
        return false;
    Manager::the()->acquire();
    return true;
}

bool release()
{
    return Manager::the()->release();
}

void setPrefix(const std::string &dir)
{
    return Manager::the()->setPrefix(dir);
}

bool isConfigured()
{
    return Manager::the()->exists();
}

Accessor::Accessor()
{
    config::acquire();
}

Accessor::Accessor(const std::string &cluster, int rank)
{
    config::init(cluster, rank);
}

void Accessor::setPrefix(const std::string &dir)
{
    config::setPrefix(dir);
}

Accessor::~Accessor()
{
    config::release();
}

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
