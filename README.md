CovConfig - COVISE, OpenCOVER & Vistle Configuration Subsystem
==============================================================

License
-------

Licensed under the LGPL v2.1 or newer, see `LICENSE` for details.

Build Requirements
------------------

- **C++ compiler**:
  support for C++17 (ISO/IEC 14882:2017) is required

- **CMake**:
  at least 3.15

- **toml++**:
  [toml for C++](https://marzer.github.io/tomlplusplus/index.html) is used for reading and writing the TOML files, included as a `git submodule`.

This library is intended to be used as a `git submodule` from the main source repository.
By `#define`'ing `CONFIG_NAMESPACE` you can put all the libraries classes into the namespace `CONFIG_NAMESPACE::config`. This mechanism is used to
keep configuration of [COVISE](https://www.hlrs.de/covise/), [OpenCOVER](https://www.hlrs.de/opencover/) and [Vistle](https://vistle.io) separated.

Usage
-----

- the library is not thread-safe
- initiate access to the config subsystem with `Access` (`#include <access.h>`)
- access values from configuration with `Value` template, `typedef`ed to `ConfigBool`, `ConfigInt`, `ConfigFloat` and `ConfigString` (`#include <value.h>`)
- access homogeneous arrays of values from configuration with `Array` template, `typedef`ed to `ConfigBoolArray`, `ConfigIntArray`, `ConfigFloatArray` and `ConfigStringArray` (`#include <array.h>`)
- modification of values/arrays is possible, will be stored to user configuration directory when saving of configuration path is requested
- install an update handler on `Value`s and `Array`s for being notified when values are changed from within same process
- existing sections and entries can be queried with `File` (`#include <file.h>`)
- revoke access with by destroying `Access`
- on UNIX, search paths follow [XDG specification](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)
- in addition, the current directory and the config subdirectory of software installation prefix are searched
- configuration in host and cluster specific subdirectories is preferred
- for every configuration path, only a single file is loaded - configuration data is not merged
- configuration is not reloaded when being changed on disk
- for getting debug output set the environment variable `COVCONFIG_DEBUG`: empty will generate all output, setting it to a non-negative level controls the amount of logging
