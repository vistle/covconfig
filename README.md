covconfig - COVISE, OpenCOVER & Vistle Configuration Subsystem
==============================================================

License
-------

Licensed under the LGPL v2.1, see `LICENSE` for details.

Build Requirements
------------------

- **C++ compiler**:
  support for C++17 (ISO/IEC 14882:2017) is required

- **CMake**:
  at least 3.15

- **toml++**:
  [toml for C++](https://marzer.github.io/tomlplusplus/index.html) is used for reading and writing the TOML files, included as a `git submodule`.

This library is intended to be used as a `git submodule` from the main source repository.

Usage
-----

- initiate access to the config subsystem with `init(...)` or `Accessor`
- access values from configuration with `Value` template, `typedef`ed to `ConfigBool`, `ConfigInt`, `ConfigFloat` and `ConfigString`
- access homogeneous arrays of values from configuration with `Array` template, `typedef`ed to `ConfigBoolArray`, `ConfigIntArray`, `ConfigFloatArray` and `ConfigStringArray`
- modification of values/arrays is possible, will be stored to user configuration directory when saving of configuration path is requested
- install a handler for being notified when values are changed from within same process
- configuration is not reloaded when being changed on disk
- revoke access with `release()` or by destroying `Accessor`
- on UNIX, search paths follow [XDG specification](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)
- in addition, current directory and the config subdirectory of software installation prefix are searched
