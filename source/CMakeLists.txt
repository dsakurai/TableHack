set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(html_clipboard)

add_executable(example)

target_sources(example
    PRIVATE
    main.cpp
    MinimalTableWidget.cpp
    MinimalTableWidget.h
    TableWidget.h
    TableWidget.cpp

    FullBackup.cpp
    FullBackup.h
    BruteForceUndoCommand.cpp
    BruteForceUndoCommand.h
    UndoableTableWidget.cpp
    UndoableTableWidget.h
)

target_link_libraries(example PUBLIC Qt6::Widgets html_clipboard)

# Find windeployqt (tries CMAKE_PREFIX_PATH, or use QT_BIN env to override)
find_program(WINDEPLOYQT_EXE NAMES windeployqt HINTS ${CMAKE_PREFIX_PATH} ENV QT_BIN)

if (NOT WINDEPLOYQT_EXE)
    message(WARNING "windeployqt not found. Set CMAKE_PREFIX_PATH or QT_BIN.")
endif()

set(DEPLOY_DIR "${CMAKE_BINARY_DIR}/deploy/release/example-portable")

add_custom_command(
    OUTPUT "${DEPLOY_DIR}/deployed.timestamp"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${DEPLOY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:example> "${DEPLOY_DIR}/$<TARGET_FILE_NAME:example>"
    COMMAND ${WINDEPLOYQT_EXE} --release --dir "${DEPLOY_DIR}" "${DEPLOY_DIR}/$<TARGET_FILE_NAME:example>"
    COMMAND ${CMAKE_COMMAND} -E touch "${DEPLOY_DIR}/deployed.timestamp"
    DEPENDS example
    COMMENT "Running windeployqt -> ${DEPLOY_DIR}"
    VERBATIM
)

add_custom_target(deploy
    DEPENDS "${DEPLOY_DIR}/deployed.timestamp"
)

# # Package into a ZIP by archiving the deploy folder
# add_custom_target(package-deploy
#     COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '${DEPLOY_DIR}\\*' -DestinationPath '${CMAKE_BINARY_DIR}\\example-portable.zip' -Force"
#     DEPENDS deploy
#     COMMENT "Creating zip ${CMAKE_BINARY_DIR}/example-portable.zip from ${DEPLOY_DIR}"
# )