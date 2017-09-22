cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(electronLunaInstaller)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (WIN32 OR MINGW)
    set(CONSOLE_INSTALLER_FILENAME "consoleInstaller.exe")
else()
    set(CONSOLE_INSTALLER_FILENAME "consoleInstaller")
endif()

if (APPLE)
	set(ELECTRON_INSTALLER_APP_NAME "lunaInstaller")
	set(PACKAGED_APP_RESOURCES_PATH "${CMAKE_CURRENT_BINARY_DIR}/lunaInstallerApp/lunaInstaller.app/Contents/Resources/app")
else()
	set(ELECTRON_INSTALLER_APP_NAME "electronLunaInstaller")
	set(PACKAGED_APP_RESOURCES_PATH "${CMAKE_CURRENT_BINARY_DIR}/lunaInstallerApp/resources/app")
endif()

set(APPLICATION_ICON "${CMAKE_SOURCE_DIR}/src/lunaInstallerApp/img/luna_logo")


# cache variables
set (CONSOLE_INSTALLER_EXE "${CMAKE_SOURCE_DIR}/src/consoleInstallerMocks/${CONSOLE_INSTALLER_FILENAME}"
    CACHE STRING "Path to console installer executable")
set (PYTHON_EXE "" CACHE STRING "Path to python executable") # used to qrc generation
set (NPM_EXE "" CACHE STRING "Path to npm executable")
set (ELECTRON_PACKAGER_EXE "" CACHE STRING "Path to electron-packager executable")
set (SASS_EXE "sassc" CACHE STRING "Path to sass preprocessor")

################ PART 1: create luna electron app

# check for npm
if (NOT NPM_EXE)
    if (UNIX)
        set (NPM_EXE "npm")
    elseif (MINGW)
        set (NPM_EXE "/c/Program Files/nodejs/npm")
    endif()
endif(NOT NPM_EXE)

# check for electron-packager
if (NOT ELECTRON_PACKAGER_EXE)
    if (UNIX)
        set (ELECTRON_PACKAGER_EXE "electron-packager")
    elseif (MINGW)
        set (ELECTRON_PACKAGER_EXE "/c/Users/$ENV{USER}/AppData/Roaming/npm/electron-packager")
    endif()
endif(NOT ELECTRON_PACKAGER_EXE)

file(GLOB_RECURSE APP_SOURCES ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp/*)

add_custom_target(lunaInstallerDeps # example of electron app dependency
    COMMAND "${NPM_EXE}" install
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp
    COMMENT "Installing luna installer electron app dependencies"
    SOURCES ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp/package.json
    )

add_custom_command(OUTPUT lunaInstallerApp/version
    COMMAND "${ELECTRON_PACKAGER_EXE}" ${CMAKE_SOURCE_DIR}/src/lunaInstallerApp ${ELECTRON_INSTALLER_APP_NAME} --overwrite --icon="${APPLICATION_ICON}"
    COMMAND [ -d lunaInstallerApp ] && rm -r lunaInstallerApp || :
    COMMAND mv ${ELECTRON_INSTALLER_APP_NAME}-* lunaInstallerApp
    DEPENDS ${APP_SOURCES} lunaInstallerDeps
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Packaging luna installer electron app")

add_custom_command(OUTPUT {PACKAGED_APP_RESOURCES_PATH}/${CONSOLE_INSTALLER_FILENAME}
    COMMAND ${CMAKE_COMMAND} -E copy "${CONSOLE_INSTALLER_EXE}" "${PACKAGED_APP_RESOURCES_PATH}/${CONSOLE_INSTALLER_FILENAME}"
    DEPENDS lunaInstallerApp/version
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying console installer to package")

add_custom_command(OUTPUT {PACKAGED_APP_RESOURCES_PATH}/installer.css
    COMMAND ${SASS_EXE} installer.scss installer.css
    DEPENDS lunaInstallerApp/version
    WORKING_DIRECTORY ${PACKAGED_APP_RESOURCES_PATH}
    COMMENT "Creating installer.css")

add_custom_target(lunaInstallerApp ALL
    DEPENDS lunaInstallerApp/version lunaInstallerDeps {PACKAGED_APP_RESOURCES_PATH}/${CONSOLE_INSTALLER_FILENAME} {PACKAGED_APP_RESOURCES_PATH}/installer.css
    SOURCES ${APP_SOURCES})

################ PART 2: On linux and windows: compile special electron app extractor

if(NOT APPLE)

    # Instruct CMake to run moc automatically when needed
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    set(CMAKE_CXX_STANDARD 11)

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
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
	if (MINGW)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
	endif(MINGW)

    # QRC

    # check for python first
    if (NOT PYTHON_EXE)
        if (UNIX)
            set (PYTHON_EXE "python2.7")
        elseif (MINGW)
        # MSYS in theory have python in its repositories, but I did not succeeded in installing psutils
        # in that environment. The Window's python installation seems to work fine
            set (PYTHON_EXE "/c/Python27/python")
        endif()
    endif(NOT PYTHON_EXE)
	
    add_custom_command(OUTPUT lunaInstallerApp.qrc
        COMMAND ${PYTHON_EXE} ${CMAKE_SOURCE_DIR}/scripts/generateQrc.py
        MAIN_DEPENDENCY scripts/generateQrc.py
        DEPENDS lunaInstallerApp ${APP_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating QRC")


    # sources

    if(QT_STATIC_INSTALL_DIRECTORY)
        set(LINKED_PLUGINS_CPP src/linkedPlugins.cpp)
    endif(QT_STATIC_INSTALL_DIRECTORY)

    if(UNIX)
        add_custom_command(OUTPUT lunaInstaller.o
            COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/lunaInstaller.o electronLunaInstaller
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

        add_custom_command(OUTPUT consoleInstaller.o
            COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/consoleInstaller.o consoleInstaller
            DEPENDS lunaInstallerApp
            WORKING_DIRECTORY ${PACKAGED_APP_RESOURCES_PATH}
            COMMENT "Create consoleInstaller.o")

        set(EMBEDDED_FILES
            src/embeddedFilesLinux.cpp
            lunaInstaller.o
            libnode.o
            consoleInstaller.o
            )
    elseif(MINGW)
        add_custom_command(OUTPUT lunaInstaller.obj
            COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/lunaInstaller.obj electronLunaInstaller.exe
            DEPENDS lunaInstallerApp
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lunaInstallerApp
            COMMENT "Create lunaInstaller.obj"
            )

        add_custom_command(OUTPUT libnode.obj
            COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/libnode.obj node.dll
            DEPENDS lunaInstallerApp
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lunaInstallerApp
            COMMENT "Create libnode.obj"
            )

        add_custom_command(OUTPUT consoleInstaller.obj
            COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/consoleInstaller.obj consoleInstaller.exe
            DEPENDS lunaInstallerApp
            WORKING_DIRECTORY ${PACKAGED_APP_RESOURCES_PATH}
            COMMENT "Create consoleInstaller.obj")

        set(EMBEDDED_FILES
            src/embeddedFilesLinux.cpp
            lunaInstaller.obj
            libnode.obj
            consoleInstaller.obj
            )
    elseif(WIN32)
        message(WARNING "Windows is not supported yet; try build in MSYS2 environment. Won't compile")
    else()
        message(FATAL_ERROR "System is not supported; the only supported os are linux, windows and osx")
    endif()

    add_executable(lunaInstaller WIN32
        src/embeddedFiles.h
        src/errors.h
        src/files.h
        src/files.cpp
        src/main.cpp
        ${LINKED_PLUGINS_CPP}
        ${EMBEDDED_FILES}
        lunaInstallerApp.qrc
        )

    set_target_properties(lunaInstaller PROPERTIES AUTOGEN_TARGET_DEPENDS lunaInstallerApp.qrc)

    qt5_use_modules(lunaInstaller Core Widgets)

    target_link_libraries(lunaInstaller
        ${QT_LIBRARIES}
        ${ICU_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

endif(NOT APPLE)
