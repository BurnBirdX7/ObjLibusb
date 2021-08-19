# ObjLibusb

Objective Wrapper for libusb

## Dependencies
 * [libusb(github)](https://github.com/libusb/libusb)

You need to install libusb

### apt
```shell
sudo apt install libusb-1.0-0-dev
```

### vcpkg
```shell
vcpkg install libusb
```

### ...other
You can install libusb any other way

## Preparation

Clone project's repository and go to its root.
```shell
git clone https://github.com/BurnBirdX7/ObjLibusb.git
cd ObjLibusb
```

## Build

```shell
mkdir build && cd build             # make dir for build and go there
cmake -DCMAKE_BUILD_TYPE=Release .. # generate project
cmake --build .                     # build
```

### vcpkg
if you use **vcpkg** you may need to set `CMAKE_TOOLCHAIN_FILE`
to `[vcpkg root]/scripts/buildsystems/vcpkg.cmake` on generation step.:
```shell
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..
```

### MSVC
When compiler is MSVC you need to specify configuration on building step:
```shell
cmake --build . --config Release
```

### ...other
if CMake cannot find libusb's package
you can set direct paths via variables `LIBUSB_INCLUDE_DIR` and `LIBUSB_LIBRARY`.\
It will be (*project generation step*)
```shell
cmake -DLIBUSB_INCLUDE_DIR=[path_to_include] -DLIBUSB_LIBRARY=[path_to_binary] ..
```

## Install
```shell
cmake --install .
```
You may need administrator's privileges to execute this command.

## Use
```cmake
find_package(ObjLibusb REQUIRED)
target_link_libraries(MyTarget PUBLIC ObjLibusb)
```
If CMake cannot find installed package, add path to ObjLibusb's installation dir to `CMAKE_PREFIX_PATH` variable:
```cmake
set(CMAKE_PREFIX_PATH "[objlibusb's installation path];${CMAKE_PREFIX_PATH}")
```
(On Windows default installation path is `C:/Program Files (x86)/ObjLibusb`)

### Without installation

You can just link built binary to your target
```cmake
target_link_libraries(MyTarget PUBLIC [path to build dir]/ObjLibusb.a) # can be .lib or any other
target_include_directories(MyTarget PUBLIC [path to objlibusb's repo]/include)
```

## Examples
### Windows (MSVC)
 * **libusb** installed via vcpkg
```shell
> git clone https://github.com/BurnBirdX7/ObjLibusb
> cd .\ObjLibusb\
> mkdir build
> cd .\build\
> cmake -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake ..
> cmake --build . --config Release
# to build library only:
> cmake --build . --config Release --target adblib
```

### Linux
* **libusb** installed with apt
```shell
$ git clone https://github.com/BurnBirdX7/ObjLibusb
$ cd ObjLibusb/
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build .
```