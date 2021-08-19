cmake_minimum_required(VERSION 3.15)
project(ObjLibusb)


message(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

set(ObjLibusb_VERSION 1.0)

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

set(source_dir src)
set(source
        ${source_dir}/Device.cpp
        ${source_dir}/Context.cpp
        ${source_dir}/DeviceList.cpp
        ${source_dir}/Error.cpp
        ${source_dir}/DeviceHandle.cpp
        ${source_dir}/Transfer.cpp)

set(cmake_config
        cmake/ObjLibusbConfig.cmake)

set(lib_destination
        lib/ObjLibusb-${ObjLibusb_VERSION})

set(headers_destination
        include/ObjLibusb-${ObjLibusb_VERSION})

set(cmake_destination
        ${lib_destination})

add_library(ObjLibusb
        ${source})

# build:
if (MSVC) # MSVC
    message(NOTICE "Trying to find libusb...")
    find_package(libusb CONFIG)

    if (NOT ${libusb_FOUND})
        message(NOTICE "libusb not found with find_package()...")
        message(NOTICE "checking supplied variables...")

        set(LIBUSB_INCLUDE_DIRS ${LIBUSB_INCLUDE_DIR})
        set(LIBUSB_LIBRARIES ${LIBUSB_LIBRARY})
    endif()

    if ((LIBUSB_INCLUDE_DIRS STREQUAL "") OR ("LIBUSB_LIBRARIES" STREQUAL ""))
        message(SEND_ERROR "libusb paths are not set")
    else()
        if (EXISTS "${LIBUSB_INCLUDE_DIRS}/libusb.h")
            message("Strange vcpkg libusb include path detected...")
            set(LIBUSB_INCLUDE_DIRS "${LIBUSB_INCLUDE_DIRS}/../")
        endif()

        message(NOTICE "libusb include dirs: ${LIBUSB_INCLUDE_DIRS}")
        message(NOTICE "libusb library dirs: ${LIBUSB_LIBRARIES}")
    endif()

    target_link_libraries(ObjLibusb PUBLIC ${LIBUSB_LIBRARIES})
    target_include_directories(ObjLibusb PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${headers_dir}>
            $<INSTALL_INTERFACE:${headers_destination}>
            ${LIBUSB_INCLUDE_DIRS})

else() # NOT MSVC

    find_package(Threads REQUIRED)
    target_link_libraries(ObjLibusb PUBLIC usb-1.0 Threads::Threads)
    target_include_directories(ObjLibusb PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${headers_dir}>
            $<INSTALL_INTERFACE:${headers_destination}>)

endif()

#install
install(TARGETS ObjLibusb EXPORT ObjLibusb DESTINATION ${lib_destination}/${CMAKE_BUILD_TYPE})
install(EXPORT ObjLibusb DESTINATION ${lib_destination}/${CMAKE_BUILD_TYPE})

install(FILES ${headers_dir}/ObjLibusb.hpp DESTINATION ${headers_destination})  # headers...
install(DIRECTORY ${headers_dir}/ObjLibusb DESTINATION ${headers_destination})  # ...headers
install(FILES ${cmake_config} DESTINATION ${cmake_destination})                 # package
