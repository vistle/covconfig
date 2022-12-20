#ifndef CONFIG_OBSERVER_H
#define CONFIG_OBSERVER_H

#include "export.h"
#include <cstdlib>

#ifdef CONFIG_NAMESPACE
namespace CONFIG_NAMESPACE {
#endif

namespace config {

class ValueObserver {
public:
    virtual void update() = 0;
};

class ArrayObserver {
public:
    virtual void update(size_t index) = 0; //< called with new size or index of changed value
};

} // namespace config
#ifdef CONFIG_NAMESPACE
}
#endif
#endif
