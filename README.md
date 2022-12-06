# HEBench Common Library

## Table of Contents
1. [About the HEBench Common Library](#about)
2. [Requirements](#requirements1)
3. [Build Configuration](#build-configuration)
   1. [Build Type](#build-type)
4. [Building](#building)
5. [Using the Common Library](#using-the-common-lib)
6. [Contributing](#contributing)

## About the HEBench Common Library <a name="about"></a>
The HEBench Common Library is a collection of unrelated modulues meant to ease development. The modules inside are tools that have been deemed useful due to continued prior use. As such, they've been collected and made available through the "common-lib" interface.

The modulues inside include:
1. args_parser: Provides a developer with full command line argument parsing functionality using the C and C++ argument format.
2. config_reader: Provides a developer with full configuration file parsing functionality for program input.
3. logging: Provides a developer with full logging functionality to log anything from an application.
4. threading: Provides a developer with various tools for proper threading within an application (pool management, clean-up, etc.).
5. timer: Provides a developer with operations to track time of fully customizable code segments.
6. general: A collection of useful tools, algorithms, and more unrelated to the main tools listed above.

## Requirements <a name="requirements1"></a>
Current build system uses Cmake.

- Ubuntu 16.04/18.04/20.04
- C++17 capable compiler (tested with GCC 9.3)
- CMake 3.12

## Build Configuration <a name="build-configuration"></a>

### Build Type <a name="build-type"></a>

If no build type is specified, the build system will build in <b>Debug</b> mode. Use `-DCMAKE_BUILD_TYPE` configuration variable to set your preferred build type:

- `-DCMAKE_BUILD_TYPE=Debug` : debug mode (default if no build type is specified).
- `-DCMAKE_BUILD_TYPE=Release` : release mode. Compiler optimizations for release enabled.
- `-DCMAKE_BUILD_TYPE=RelWithDebInfo` : release mode with debug symbols.
- `-DCMAKE_BUILD_TYPE=MinSizeRel` : release mode optimized for size.

## Building <a name="building"></a>
Build from the top level of `common-lib` with Cmake as follows:

```bash
# assuming common-lib is already cloned
cd ~/common-lib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_LOCATION # change install location at will
make -j
make install # install built components
```

The install step will copy the target common-lib library `libhebench_common-lib.a` to `$INSTALL_LOCATION/lib` and the cmmon-lib headders to `$INSTALL_LOCATION/include`

## Using the Common Library <a name="using-the-common-lib"></a>

Once the common-lib has been built and installed successfully (assuming it has been installed to `$INSTALL_LOCATION`), an application/library can use the HEBench Common Library as follows:

```bash
# Add the following to the library/application (<target>) in question's cmake
target_link_libraries(<target> PRIVATE hebench_common-lib)
```

## Contributing <a name="contributing"></a>

This project welcomes external contributions. To contribute to HEBench, see [CONTRIBUTING.md](CONTRIBUTING.md). We encourage feedback and suggestions via [Github Issues](https://github.com/hebench/common-lib/issues) as well as discussion via [Github Discussions](https://github.com/hebench/common-lib/discussions).

