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

# define build variables
SET(ELEKTRON_APP_RESOURCE "${CMAKE_SOURCE_DIR}/src/lunaInstallerApp.qrc" CACHE STRING "Path to qrc with console installer executable")
message("Will be linked with resource ${ELEKTRON_APP_RESOURCE}")

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

# sources

add_executable(elektronInstaller
    src/main.cpp
    ${ELEKTRON_APP_RESOURCE}
    )

qt5_use_modules(elektronInstaller Core)

target_link_libraries(elektronInstaller
    ${QT_LIBRARIES}
    ${ICU_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    )
