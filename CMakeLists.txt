cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(lunaInstaller)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

################ PART 1: create luna electron app

file(GLOB_RECURSE APP_SOURCES ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp/*)

add_custom_target(lunaInstallerDeps # example of electron app dependency
    COMMAND npm install
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp
    COMMENT "Installing luna installer electron app dependencies"
    SOURCES ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp/package.json
    )

add_custom_command(OUTPUT lunaInstallerApp/version
    COMMAND electron-packager ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp lunaInstaller --overwrite
    COMMAND [ -d lunaInstallerApp ] && rm -r lunaInstallerApp || :
    COMMAND mv lunaInstaller-* lunaInstallerApp
    DEPENDS ${APP_SOURCES} lunaInstallerDeps
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Packaging luna installer electron app")

add_custom_target(lunaInstallerApp ALL
    DEPENDS lunaInstallerApp/version lunaInstallerDeps
    SOURCES ${APP_SOURCES})

################ PART 2: On linux and windows: compile special electron app extractor

if(NOT APPLE)

    # Instruct CMake to run moc automatically when needed
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOMOC_MOC_OPTIONS "-b;stdafx.h")
    set(CMAKE_AUTORCC ON)

    set(CMAKE_CXX_STANDARD 14)

    # find dependencies
    include(findDependencies.cmake)

    # global included directories
    include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${Qt5_INCLUDES}
        )

    # global compiler and linker flags
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            set(WARNING_FLAGS
                -Wall
                -Wno-unused-parameter
                -Wno-parentheses
                -Wno-unused-variable
                -Wno-sign-compare
                -Wno-potentially-evaluated-expression
                -Wno-undefined-var-template
                -Wno-null-conversion
                -Wno-pointer-bool-conversion
                -Werror=return-type
                -Werror=narrowing
                -Werror
                )
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set(WARNING_FLAGS
                    -Wall
                    -Wno-unused-parameter
                    -Wno-parentheses
                    -Wno-unused-variable
                    -Wno-unused-but-set-variable
                    -Wno-sign-compare
                    -Werror=return-type
                    -Werror=narrowing
                    )
    elseif()
            message("Compiler ${CMAKE_CXX_COMPILER_ID} not supported!")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed)

    add_custom_command(OUTPUT lunaInstallerApp.qrc
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/generateQrc.py
        MAIN_DEPENDENCY scripts/generateQrc.py
        DEPENDS lunaInstallerApp ${APP_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating QRC")


    # sources

    if(QT_STATIC_INSTALL_DIRECTORY)
        set(LINKED_PLUGINS_CPP src/linkedPlugins)
    endif(QT_STATIC_INSTALL_DIRECTORY)

    if(UNIX)
        add_custom_command(OUTPUT lunaInstaller.o
            COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/lunaInstaller.o lunaInstaller
            DEPENDS lunaInstallerApp
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lunaInstallerApp
            COMMENT "Create lunaInstaller.o"
            )

        add_custom_command(OUTPUT libnode.o
            COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/libnode.o libnode.so
            DEPENDS lunaInstallerApp
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lunaInstallerApp
            COMMENT "Create libnode.o"
            )

        set(EMBEDDED_FILES
            src/embeddedFilesLinux.cpp
            lunaInstaller.o
            libnode.o
            )
    elseif(WIN32)
        message(WARNING "Windows is not supported yet")
    else()
        message(FATAL_ERROR "System is not supported; the only supported os are linux, windows and osx")
    endif()

    add_executable(electronInstaller
        src/embeddedFiles.h
        src/errors.h
        src/files.h
        src/files.cpp
        src/main.cpp
        ${LINKED_PLUGINS_CPP}
        ${EMBEDDED_FILES}
        lunaInstallerApp.qrc
        )

    set_target_properties(electronInstaller PROPERTIES AUTOGEN_TARGET_DEPENDS lunaInstallerApp.qrc)

    qt5_use_modules(electronInstaller Core Widgets)

    target_link_libraries(electronInstaller
        ${QT_LIBRARIES}
        ${ICU_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

endif(NOT APPLE)
