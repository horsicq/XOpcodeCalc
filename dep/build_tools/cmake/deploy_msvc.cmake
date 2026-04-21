message(STATUS ${MSVC_TOOLSET_VERSION})
if(MSVC)
    set(_deploy_msvc_destination ".")
    set(_deploy_msvc_runtime_installed FALSE)

    # Let CMake discover the MSVC redist files from the selected toolchain.
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${_deploy_msvc_destination}")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    include(InstallRequiredSystemLibraries)

    foreach(_deploy_msvc_runtime_lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        get_filename_component(_deploy_msvc_runtime_name "${_deploy_msvc_runtime_lib}" NAME)

        if(_deploy_msvc_runtime_name STREQUAL "msvcp140.dll"
            OR _deploy_msvc_runtime_name STREQUAL "msvcp140_1.dll"
            OR _deploy_msvc_runtime_name STREQUAL "msvcp140_2.dll"
            OR _deploy_msvc_runtime_name STREQUAL "vcruntime140.dll"
            OR _deploy_msvc_runtime_name STREQUAL "vcruntime140_1.dll")
            install(FILES "${_deploy_msvc_runtime_lib}" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
            set(_deploy_msvc_runtime_installed TRUE)
        endif()
    endforeach()

    # Keep the old Visual Studio 2013 layout as a fallback.
    if((NOT _deploy_msvc_runtime_installed) AND CMAKE_GENERATOR MATCHES "Visual Studio 12 2013")
        set(_deploy_msvc_redist_dir "$ENV{VCINSTALLDIR}/redist/x86/Microsoft.VC120.CRT")
        string(REPLACE "\\" "/" _deploy_msvc_redist_dir "${_deploy_msvc_redist_dir}")
        message(STATUS "${_deploy_msvc_redist_dir}")

        install(FILES "${_deploy_msvc_redist_dir}/msvcp120.dll" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
        install(FILES "${_deploy_msvc_redist_dir}/msvcr120.dll" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
        set(_deploy_msvc_runtime_installed TRUE)
    endif()

    # Fall back to the legacy env-var-based redist path when CMake discovery
    # is unavailable, for example in older toolchain setups.
    if((NOT _deploy_msvc_runtime_installed) AND (MSVC_TOOLSET_VERSION GREATER_EQUAL 142))
        set(_deploy_msvc_platform "$ENV{Platform}")
        if("${_deploy_msvc_platform}" STREQUAL "")
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(_deploy_msvc_platform "x64")
            else()
                set(_deploy_msvc_platform "x86")
            endif()
        endif()

        set(_deploy_msvc_redist_dir "$ENV{VCToolsRedistDir}${_deploy_msvc_platform}/Microsoft.VC${MSVC_TOOLSET_VERSION}.CRT")
        string(REPLACE "\\" "/" _deploy_msvc_redist_dir "${_deploy_msvc_redist_dir}")
        message(STATUS "${_deploy_msvc_redist_dir}")

        install(FILES "${_deploy_msvc_redist_dir}/msvcp140.dll" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
        install(FILES "${_deploy_msvc_redist_dir}/msvcp140_1.dll" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
        install(FILES "${_deploy_msvc_redist_dir}/msvcp140_2.dll" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
        install(FILES "${_deploy_msvc_redist_dir}/vcruntime140.dll" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
        install(FILES "${_deploy_msvc_redist_dir}/vcruntime140_1.dll" DESTINATION "${_deploy_msvc_destination}" OPTIONAL)
        set(_deploy_msvc_runtime_installed TRUE)
    endif()

    if(NOT _deploy_msvc_runtime_installed)
        message(WARNING "MSVC runtime libraries were not discovered.")
    endif()
endif()
