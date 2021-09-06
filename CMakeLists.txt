cmake_minimum_required(VERSION 3.15)
project(ObjLibusb)

set(CMAKE_CXX_STANDARD 17)

### SETUP GENERAL VARIABLES ###

set(ObjLibusb_VERSION_MAJOR 1)
set(ObjLibusb_VERSION_MINOR 0)
set(ObjLibusb_VERSION_PATCH 0)
set(ObjLibusb_VERSION "${ObjLibusb_VERSION_MAJOR}.${ObjLibusb_VERSION_MINOR}.${ObjLibusb_VERSION_PATCH}")

# Headers
set(headers_dir include)
set(headers
        ${headers_dir}/ObjLibusb.hpp
        ${headers_dir}/ObjLibusb/Context.hpp
        ${headers_dir}/ObjLibusb/Descriptors.hpp
        ${headers_dir}/ObjLibusb/DeviceHandle.hpp
        ${headers_dir}/ObjLibusb/Device.hpp
        ${headers_dir}/ObjLibusb/DeviceList.hpp
        ${headers_dir}/ObjLibusb/Error.hpp
        ${headers_dir}/ObjLibusb/ObjLibusb.hpp
        ${headers_dir}/ObjLibusb/Transfer.hpp)

# Source Files
set(source_dir src)
set(source
        ${source_dir}/Device.cpp
        ${source_dir}/Context.cpp
        ${source_dir}/DeviceList.cpp
        ${source_dir}/Error.cpp
        ${source_dir}/DeviceHandle.cpp
        ${source_dir}/Transfer.cpp)

# Cmake Configuration
configure_file( cmake/ObjLibusbConfigVersion.cmake.in
                cmake/ObjLibusbConfigVersion.cmake
                @ONLY
                )

# Convenience Variables
set(cmake_config_files
        cmake/ObjLibusbConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/ObjLibusbConfigVersion.cmake)

set(lib_destination
        lib/ObjLibusb-${ObjLibusb_VERSION})

set(headers_destination
        include/ObjLibusb-${ObjLibusb_VERSION})

set(cmake_destination
        ${lib_destination})

add_library(ObjLibusb
        ${source})

### Build ###

target_include_directories(ObjLibusb PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${headers_dir}>
        $<INSTALL_INTERFACE:${headers_destination}>)



# FIND AND LINK libusb #

if (DEFINED LIBUSB_INCLUDE_DIRS AND DEFINED LIBUSB_LIBRARIES) # If user provided paths

    message(NOTICE " ~~ LIBUSB_INCLUDE_DIRS and LIBUSB_LIBRARIES are defined... There will be no attempt to find libusb...")
    message(NOTICE " ~~ ObjLibusb is going to be configured with this paths")

    target_link_libraries(ObjLibusb PUBLIC
            ${LIBUSB_LIBRARIES})
    target_include_directories(ObjLibusb PUBLIC
            ${LIBUSB_INCLUDE_DIRS})

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # Under Linux and with GCC compiler it is possible to link libusb without finding it
    # TODO: Check and test this

    target_link_libraries(ObjLibusb PUBLIC
            usb-1.0)

else () #

    message(STATUS " ~~ Trying to find libusb via find_package()")
    find_package(libusb REQUIRED)

    # Check for untypical include path
    if (EXISTS "${LIBUSB_INCLUDE_DIRS}/libusb.h")
        message(STATUS " ~~ Untypical libusb include path detected... Trying to include ${LIBUSB_INCLUDE_DIRS}/../")
        set(LIBUSB_INCLUDE_DIRS "${LIBUSB_INCLUDE_DIRS}/../")
    endif()

    target_link_libraries(ObjLibusb PUBLIC
            ${LIBUSB_LIBRARIES})
    target_include_directories(ObjLibusb PUBLIC
            ${LIBUSB_INCLUDE_DIRS})

endif ()



# FIND AND LINK THREADS #

find_package(Threads REQUIRED)
target_link_libraries(ObjLibusb PUBLIC
        Threads::Threads)


### CONFIGURE INSTALLATION ###

set(install_name "ObjLibusb_CONFIG_NAME")
set(install_name_docstring "Name of the configuration under which this of the library will be installed")

if(DEFINED CMAKE_BUILD_TYPE AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    set(${install_name} ${CMAKE_BUILD_TYPE} CACHE STRING "${install_name_docstring}")
else()
    set(${install_name} "Release" CACHE STRING "${install_name_docstring}")
endif()

set(config_name ${${install_name}})
message(STATUS " ~~ ObjLibusb configuration name: ${config_name} (This name can be used to install and later find installed configuration)")


install(TARGETS ObjLibusb EXPORT ObjLibusb DESTINATION ${lib_destination}/${config_name}) # install lib and add it to export
install(EXPORT ObjLibusb DESTINATION ${lib_destination}/${config_name})                   # install export file

install(FILES ${headers_dir}/ObjLibusb.hpp DESTINATION ${headers_destination})  # general header
install(DIRECTORY ${headers_dir}/ObjLibusb DESTINATION ${headers_destination})  # headers
install(FILES ${cmake_config_files} DESTINATION ${cmake_destination})           # cmake package configuration
