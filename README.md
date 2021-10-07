# HEBench API Bridge

## Table of Contents
1. [Requirements](#requirements1)
2. [Online Documentation](#online-documentation)
3. [API Bridge](#api-bridge)
4. [Build Configuration](#build-configuration)
   1. [Build Type](#build-type)
5. [Building](#building)
8. [Contributing](#contributing)


## Requirements <a name="requirements1"></a>
Current build system uses CMake.

- Ubuntu 16.04, 18.04, or 20.04
- C++14 capable compiler (tested with GCC 9.3)
- CMake 3.12 or above

## Online Documentation <a name="online-documentation"></a>
The complete online documentation for HEBench can be found here [HEBench online documentation](https://hebench.github.io/frontend)

## API Bridge <a name="api-bridge"></a>

This is a primary component of HEBench. For more information see [HEBench home page](https://hebench.github.io) and [frontend](https://github.com/hebench/frontend) repository.

API Bridge is the HEBench component that provides the communication between the frontend's Test Harness and backends.

It consists of a set of C headers that declare the C API used by the Test Harness to orchestrate, execute and benchmark workloads implemented by backends. It also includes a C++ wrapper which offers a collection of pre-defined classes and functionality designed to ease the creation of backends by hiding boilerplate details required by the C API.

## Build Configuration <a name="build-configuration"></a>

**Important:** If users plan to only use the C API to develop a backend, there is no need to build the C++ wrapper. Simply include the C header `api.h` into the backend project.

The following instructions provide guidance to build the C++ wrapper for backends wanting to use the it.

API Bridge is automatically cloned and built as part of the frontend's default build process, and thus, unless required for advance usage, it is not required to be manually built for frontend.

### Build Type <a name="build-type"></a>

If no build type is specified, the build system will build in <b>Debug</b> mode. Use `-DCMAKE_BUILD_TYPE` configuration variable to set your preferred build type:

- `-DCMAKE_BUILD_TYPE=Debug` : debug mode (default if no build type is specified).
- `-DCMAKE_BUILD_TYPE=Release` : release mode. Compiler optimizations for release enabled.
- `-DCMAKE_BUILD_TYPE=RelWithDebInfo` : release mode with debug symbols.
- `-DCMAKE_BUILD_TYPE=MinSizeRel` : release mode optimized for size.

## Building <a name="building"></a>

Building API Bridge will generate the libraries needed for the C++ wrapper and an example backend that shows some basic principles to extend the C++ wrapper to create a new backend.

Build from the top level of `api_bridge` with CMake as follows:

```bash
# assuming api_bridge is already cloned to home directory
cd ~/api_bridge
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_LOCATION # change install location at will
make -j
make install
```

The install step will copy the libraries, and includes into `lib` and `include`, respectively.

## Contributing <a name="contributing"></a>

This project welcomes external contributions. To contribute to HEBench, see [CONTRIBUTING.md](CONTRIBUTING.md). We encourage feedback and suggestions via [Github Issues](https://github.com/hebench/api-bridge/issues) as well as discussion via [Github Discussions](https://github.com/hebench/api-bridge/discussions).
