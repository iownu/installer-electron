cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(installer)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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


# prepare lunaInstaller exec

add_custom_command(OUTPUT lunaInstaller-linux-x64/lunaInstaller
    COMMAND electron-packager src/lunaInstallerApp lunaInstaller --overwrite --out ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Packaging luna installer electron app")

add_custom_command(OUTPUT lunaInstaller.o
    COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/lunaInstaller.o lunaInstaller
    MAIN_DEPENDENCY lunaInstaller-linux-x64/lunaInstaller
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lunaInstaller-linux-x64
    COMMENT "Create lunaInstaller.o"
    )

add_custom_command(OUTPUT lunaInstallerApp.qrc
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/generateQrc.py
    MAIN_DEPENDENCY lunaInstaller-linux-x64/lunaInstaller
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating QRC")


# sources

add_executable(electronInstaller
    src/main.cpp
    lunaInstallerApp.qrc
    lunaInstaller.o
    )

set_target_properties(electronInstaller PROPERTIES AUTOGEN_TARGET_DEPENDS lunaInstallerApp.qrc)

qt5_use_modules(electronInstaller Core)

target_link_libraries(electronInstaller
    ${QT_LIBRARIES}
    ${ICU_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    )
