# Package configuration file


get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Auxiliary function

function(ObjLibusb_NoticeAvailableConfigurations)

    set(found_configs)
    foreach(type Debug;Release;RelWithDebInfo;MinSizeRel;Default)

        if (EXISTS ${SELF_DIR}/${type}/ObjLibusb.cmake)
            list(APPEND found_configs ${type})
        endif()

    endforeach()

    list(LENGTH found_configs found_count)
    if ("${found_count}" EQUAL "0")
        message(NOTICE "~~ No installations of ObjLibusb found")
        set(example "Release")
    else()
        message(NOTICE "~~ Found configurations of Objlibusb(${found_count}): ")
        foreach(type ${found_configs})
            message(NOTICE "~~\t${type}")
        endforeach()

        list(GET found_configs 0 example)
    endif()

    message(NOTICE "~~ * Configurations with non-standard names might not be listed")
    message(NOTICE "~~ You can set desired configuration via ObjLibusb_REQUIRED_CONFIG variable, for example: ")
    message(NOTICE "~~")
    message(NOTICE "~~\tset(ObjLibusb_REQUIRED_CONFIG ${example})")
    message(NOTICE "~~\tfind_package(ObjLibusb)")
    message(NOTICE "~~")
    message(NOTICE "~~ If user doesn't set ObjLibusb_REQUIRED_CONFIG then it is set to CMAKE_BUILD_TYPE, ")
    message(NOTICE "~~\tif CMAKE_BUILD_TYPE is not defined (or is empty string) ObjLibusb_REQUIRED_CONFIG is set to 'Release'")
    message(NOTICE "~~")

endfunction()


### You can set ObjLibusb_REQUIRED_CONFIG to specify which config you want to install ###
# CMAKE_BUILD_TYPE will be used to search config if ObjLibusb_REQUIRED_CONFIG is not defined #


if (DEFINED ObjLibusb_REQUIRED_CONFIG AND NOT ${ObjLibusb_REQUIRED_CONFIG} STREQUAL "")
    message(NOTICE "~~ ObjLibusb_REQUIRED_CONFIG is set to: ${ObjLibusb_REQUIRED_CONFIG}.")
elseif(DEFINED CMAKE_BUILD_TYPE AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    message(NOTICE "~~ ObjLibusb_REQUIRED_CONFIG is not set. Setting it to CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'.")
    set(ObjLibusb_REQUIRED_CONFIG ${CMAKE_BUILD_TYPE})
else()
    message(NOTICE "~~ Neither ObjLibusb_REQUIRED_CONFIG nor CMAKE_BUILD_TYPE are set. Setting ObjLibusb_REQUIRED_CONFIG to 'Release'.")
    set(ObjLibusb_REQUIRED_CONFIG Release)
endif()


if (DEFINED ObjLibusb_REQUIRED_CONFIG AND EXISTS ${SELF_DIR}/${ObjLibusb_REQUIRED_CONFIG}/ObjLibusb.cmake)
    include(${SELF_DIR}/${ObjLibusb_REQUIRED_CONFIG}/ObjLibusb.cmake)
else()
    message(NOTICE "~~ Required configuration does not exist")
    ObjLibusb_NoticeAvailableConfigurations()
    message(SEND_ERROR "[ObjLibusb] Unavailable configuration was requested")
endif()
