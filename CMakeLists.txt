cmake_minimum_required(VERSION 3.19)
project(obj_libusb)


message(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

set(OBJ_LIBUSB_SRC
        LibusbDevice.cpp
        LibusbContext.cpp
        LibusbDeviceList.cpp
        LibusbError.cpp
        LibusbDeviceHandle.cpp
        )

add_executable(usb_test main.cpp ${OBJ_LIBUSB_SRC})
add_library(obj_libusb ${OBJ_LIBUSB_SRC})

find_package(Threads REQUIRED)
target_link_libraries(usb_test usb-1.0 Threads::Threads)