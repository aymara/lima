# Copyright (c) 2017 Nathan Osman
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

#find_package(QtCore REQUIRED)

# Retrieve the absolute path to qmake and then use that path to find
# the windeployqt binary
get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

# Running this with MSVC 2015 requires CMake 3.6+
if((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
        AND CMAKE_VERSION VERSION_LESS "3.6")
    message(WARNING "Deploying with MSVC 2015+ requires CMake 3.6+")
endif()

# Add commands that copy the Qt runtime to the target's output directory after
# build and install the Qt runtime to the specified directory
function(mywindeployqt)
    cmake_parse_arguments(_deploy
        ""
        "TARGET;DIRECTORY;QMLDIR"
        ""
        ${ARGN}
        )

    if(NOT _deploy_TARGET)
        message(FATAL_ERROR "A TARGET must be specified")
    endif()
    if(NOT _deploy_DIRECTORY)
        message(FATAL_ERROR "A target DIRECTORY must be specified")
    endif()
    if(_deploy_QMLDIR)
        list(APPEND _ARGS --qmldir ${_deploy_QMLDIR})
    endif()

    install(DIRECTORY "${_qt_bin_dir}/../qml/Qt" DESTINATION bin
  COMPONENT runtime)
    install(DIRECTORY "${_qt_bin_dir}/../qml/QtQml" DESTINATION bin
  COMPONENT runtime)
    install(DIRECTORY "${_qt_bin_dir}/../qml/QtQuick" DESTINATION bin
  COMPONENT runtime)
    install(DIRECTORY "${_qt_bin_dir}/../qml/QtQuick.2" DESTINATION bin
  COMPONENT runtime)

    # Run windeployqt immediately after build
    add_custom_command(TARGET ${_deploy_TARGET} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                 ${_ARGS}
                --verbose 1
                --compiler-runtime
                --angle
                -core -test -qml -quick -gui -widgets
                \"$<TARGET_FILE:${_deploy_TARGET}>\"
    )
#                    -core -test -qml -quick -gui -widgets -quickcontrols2

    # install(CODE ...) doesn't support generator expressions, but
    # file(GENERATE ...) does - store the path in a file
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${_deploy_TARGET}_path"
        CONTENT "$<TARGET_FILE:${_deploy_TARGET}>"
    )

    # Before installation, run a series of commands that copy each of the Qt
    # runtime files to the appropriate directory for installation
    install(CODE
        "
        file(READ \"${CMAKE_CURRENT_BINARY_DIR}/${_deploy_TARGET}_path\" _file)
        message(\"Calling windeployqt on '\${_file}' to install QML files\")
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E
                env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
                    \${_ARGS}
                    --dry-run
                    --compiler-runtime
                    --angle
                    --list mapping
                    -core -test -qml -quick -gui -widgets
                    \${_file}
            OUTPUT_VARIABLE _output
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        separate_arguments(_files WINDOWS_COMMAND \${_output})
        while(_files)
            list(GET _files 0 _src)
            list(GET _files 1 _dest)
            execute_process(
                COMMAND \"${CMAKE_COMMAND}\" -E
                    copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/${_deploy_DIRECTORY}/\${_dest}\"
            )
            list(REMOVE_AT _files 0 1)
        endwhile()
        "
    )

    # windeployqt doesn't work correctly with the system runtime libraries,
    # so we fall back to one of CMake's own modules for copying them over
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
    foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        get_filename_component(filename "${lib}" NAME)
        add_custom_command(TARGET ${_deploy_TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different "${lib}" \"$<TARGET_FILE_DIR:${_deploy_TARGET}>\"
        )
    endforeach()

endfunction()

mark_as_advanced(WINDEPLOYQT_EXECUTABLE)
