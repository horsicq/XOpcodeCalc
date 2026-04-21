message("macdeployqt")

find_program(MACDEPLOYQT NAMES macdeployqt)
if(MACDEPLOYQT)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${MACDEPLOYQT} "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
        COMMENT "Macdeployqt the executable"
    )
else()
    message("macdeployqt not found. Please install Qt tools")
endif()
