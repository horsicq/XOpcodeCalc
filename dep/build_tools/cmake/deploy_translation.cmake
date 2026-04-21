function(deploy_install_directory)
    set(options)
    set(oneValueArgs SOURCE_DIR INSTALL_DESTINATION WINDOWS_APPDATA_SUBDIR)
    set(multiValueArgs)
    cmake_parse_arguments(DEPLOY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEPLOY_SOURCE_DIR)
        message(WARNING "deploy_install_directory: SOURCE_DIR is empty.")
        return()
    endif()

    if(NOT EXISTS "${DEPLOY_SOURCE_DIR}")
        message(WARNING "deploy_install_directory: SOURCE_DIR does not exist: ${DEPLOY_SOURCE_DIR}")
        return()
    endif()

    if(NOT DEPLOY_INSTALL_DESTINATION)
        message(WARNING "deploy_install_directory: INSTALL_DESTINATION is empty.")
        return()
    endif()

    install(DIRECTORY "${DEPLOY_SOURCE_DIR}/" DESTINATION "${DEPLOY_INSTALL_DESTINATION}")

    if(WIN32 AND DEPLOY_WINDOWS_APPDATA_SUBDIR)
        set(_deploy_install_code "string(FIND \"\${CMAKE_INSTALL_PREFIX}\" \"_CPack_Packages\" _deploy_cpack_index)\n")
        string(APPEND _deploy_install_code "if(_deploy_cpack_index EQUAL -1)\n")
        string(APPEND _deploy_install_code "    set(_deploy_source_dir \"\${CMAKE_INSTALL_PREFIX}/${DEPLOY_INSTALL_DESTINATION}\")\n")
        string(APPEND _deploy_install_code "    if(EXISTS \"\${_deploy_source_dir}\")\n")
        string(APPEND _deploy_install_code "        if(NOT \"\$ENV{APPDATA}\" STREQUAL \"\")\n")
        string(APPEND _deploy_install_code "            file(TO_CMAKE_PATH \"\$ENV{APPDATA}\" _deploy_appdata_dir)\n")
        string(APPEND _deploy_install_code "            set(_deploy_target_dir \"\${_deploy_appdata_dir}/${DEPLOY_WINDOWS_APPDATA_SUBDIR}/${DEPLOY_INSTALL_DESTINATION}\")\n")
        string(APPEND _deploy_install_code "            file(MAKE_DIRECTORY \"\${_deploy_target_dir}\")\n")
        string(APPEND _deploy_install_code "            file(COPY \"\${_deploy_source_dir}/\" DESTINATION \"\${_deploy_target_dir}\")\n")
        string(APPEND _deploy_install_code "        endif()\n")
        string(APPEND _deploy_install_code "    endif()\n")
        string(APPEND _deploy_install_code "endif()\n")

        install(CODE "${_deploy_install_code}")
    endif()
endfunction()

function(deploy_add_translations)
    set(options ADD_TO_ALL)
    set(oneValueArgs TARGET_NAME INSTALL_DESTINATION OUTPUT_DIR WINDOWS_APPDATA_SUBDIR)
    set(multiValueArgs TS_FILES LRELEASE_HINTS)
    cmake_parse_arguments(DEPLOY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEPLOY_TARGET_NAME)
        set(DEPLOY_TARGET_NAME translations)
    endif()

    if(NOT DEPLOY_INSTALL_DESTINATION)
        set(DEPLOY_INSTALL_DESTINATION translations)
    endif()

    if(NOT DEPLOY_OUTPUT_DIR)
        set(DEPLOY_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/translations")
    endif()

    if(NOT DEPLOY_TS_FILES)
        message(WARNING "deploy_add_translations: TS_FILES is empty.")
        return()
    endif()

    set(_lrelease_hints ${DEPLOY_LRELEASE_HINTS})
    if(CMAKE_PREFIX_PATH)
        set(_qt_prefix_paths ${CMAKE_PREFIX_PATH})
        list(GET _qt_prefix_paths 0 _qt_prefix_first)
        list(APPEND _lrelease_hints "${_qt_prefix_first}/bin")
    endif()

    find_program(_deploy_lrelease_executable lrelease HINTS ${_lrelease_hints})

    if(NOT _deploy_lrelease_executable)
        message(WARNING "lrelease was not found. Translation files will not be generated.")
        return()
    endif()

    set(_deploy_qm_files "")
    foreach(_deploy_ts_file ${DEPLOY_TS_FILES})
        get_filename_component(_deploy_ts_name "${_deploy_ts_file}" NAME_WE)
        set(_deploy_qm_file "${DEPLOY_OUTPUT_DIR}/${_deploy_ts_name}.qm")

        add_custom_command(
            OUTPUT "${_deploy_qm_file}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_OUTPUT_DIR}"
            COMMAND "${_deploy_lrelease_executable}" "${_deploy_ts_file}" -qm "${_deploy_qm_file}"
            DEPENDS "${_deploy_ts_file}"
            VERBATIM
        )

        list(APPEND _deploy_qm_files "${_deploy_qm_file}")
    endforeach()

    if(_deploy_qm_files)
        if(DEPLOY_ADD_TO_ALL)
            add_custom_target(${DEPLOY_TARGET_NAME} ALL DEPENDS ${_deploy_qm_files})
        else()
            add_custom_target(${DEPLOY_TARGET_NAME} DEPENDS ${_deploy_qm_files})
        endif()

        install(FILES ${_deploy_qm_files} DESTINATION ${DEPLOY_INSTALL_DESTINATION})

        if(WIN32 AND DEPLOY_WINDOWS_APPDATA_SUBDIR)
            set(_deploy_install_code "string(FIND \"\${CMAKE_INSTALL_PREFIX}\" \"_CPack_Packages\" _deploy_cpack_index)\n")
            string(APPEND _deploy_install_code "if(_deploy_cpack_index EQUAL -1)\n")
            string(APPEND _deploy_install_code "    set(_deploy_source_dir \"\${CMAKE_INSTALL_PREFIX}/${DEPLOY_INSTALL_DESTINATION}\")\n")
            string(APPEND _deploy_install_code "    if(EXISTS \"\${_deploy_source_dir}\")\n")
            string(APPEND _deploy_install_code "        if(NOT \"\$ENV{APPDATA}\" STREQUAL \"\")\n")
            string(APPEND _deploy_install_code "            file(TO_CMAKE_PATH \"\$ENV{APPDATA}\" _deploy_appdata_dir)\n")
            string(APPEND _deploy_install_code "            set(_deploy_target_dir \"\${_deploy_appdata_dir}/${DEPLOY_WINDOWS_APPDATA_SUBDIR}/${DEPLOY_INSTALL_DESTINATION}\")\n")
            string(APPEND _deploy_install_code "            file(MAKE_DIRECTORY \"\${_deploy_target_dir}\")\n")
            string(APPEND _deploy_install_code "            file(COPY \"\${_deploy_source_dir}/\" DESTINATION \"\${_deploy_target_dir}\")\n")
            string(APPEND _deploy_install_code "        endif()\n")
            string(APPEND _deploy_install_code "    endif()\n")
            string(APPEND _deploy_install_code "endif()\n")

            install(CODE "${_deploy_install_code}")
        endif()

        set(${DEPLOY_TARGET_NAME}_QM_FILES ${_deploy_qm_files} PARENT_SCOPE)
    endif()
endfunction()
