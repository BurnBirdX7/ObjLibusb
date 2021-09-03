# Package configuration file


get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Auxiliary function

function(ObjLibusb_NoticeAvailableConfigurations)

    set(types_str "")
    foreach(type Unspecified;Debug;Release;RelWithDebInfo;MinSizeRel;Default)

        if (EXISTS ${SELF_DIR}/${type}/ObjLibusb.cmake)
            set(types_str "${types_str}${type};")
        endif()

    endforeach()

    if (${types_str} STREQUAL "")
        message(NOTICE "No installations of ObjLibusb found")
    else()
        message(NOTICE "Installations of ObjLibusb found:")
        message(NOTICE "\t${types_str}")
    endif()

    message(NOTICE " -- Configurations with non-default names might not be displayed here -- ")
    message(NOTICE "You can set required configurations via ObjLibusb_REQUIRED_CONFIG variable, for example: ")
    message(NOTICE "\t# ...")
    message(NOTICE "\tset(ObjLibusb_REQUIRED_CONFIG Release)")
    message(NOTICE "\t# ...")

    unset(types_str)

endfunction()


### You can set ObjLibusb_REQUIRED_CONFIG to specify which config you want to install ###
# ${CMAKE_BUILD_TYPE} will be used to search config if ObjLibusb_REQUIRED_CONFIG is not defined #

if (DEFINED ${ObjLibusb_REQUIRED_CONFIG})
    message("ObjLibusb_REQUIRED_CONFIG is set to: ${ObjLibusb_REQUIRED_CONFIG}")
elseif(DEFINED ${CMAKE_BUILD_TYPE})
    message("ObjLibusb_REQUIRED_CONFIG isn't set. Setting it to CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}...")
    set(ObjLibusb_REQUIRED_CONFIG ${CMAKE_BUILD_TYPE})
else()
    message(SEND_ERROR "Neither ObjLibusb_REQUIRED_CONFIG nor CMAKE_BUILD_TYPE are set... Cannot proceed.")
endif()


if (EXISTS ${SELF_DIR}/${ObjLibusb_REQUIRED_CONFIG}/ObjLibusb.cmake)
    include(${SELF_DIR}/${ObjLibusb_REQUIRED_CONFIG}/ObjLibusb.cmake)
    message(STATUS "ObjLibusb.cmake included")
else()
    message(NOTICE "Cannot include required configuration...")
    ObjLibusb_NoticeAvailableConfigurations()
    message(SEND_ERROR "Cannot proceed.")
endif()
