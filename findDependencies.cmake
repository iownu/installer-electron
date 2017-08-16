
#find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
#find_package(Qt5Qml REQUIRED)
#find_package(Qt5Quick REQUIRED)
find_package(ICU REQUIRED i18n uc data)
find_package(Threads REQUIRED)

message("System name: ${CMAKE_SYSTEM_NAME}")

find_path(QT_STATIC_INSTALL_DIRECTORY lib/libQt5Core.a)

if(QT_STATIC_INSTALL_DIRECTORY)
    message("Found static Qt install directory in \"${QT_STATIC_INSTALL_DIRECTORY}\"")
    set(QT_LIBRARIES
        -Wl,-Bstatic
#            -L${QT_STATIC_INSTALL_DIRECTORY}/plugins/platforms
#                -lqwayland-egl
            -L${QT_STATIC_INSTALL_DIRECTORY}/lib
#                -lqwayland-generic
#                -lqwayland-xcomposite-egl
#                -lQt5WaylandClient
#                -lqxcb
#            -L${QT_STATIC_INSTALL_DIRECTORY}/plugins/xcbglintegrations
#                -lqxcb-egl-integration
#                -lQt5XcbQpa
#                -lxcb-static
#            -L${QT_STATIC_INSTALL_DIRECTORY}/plugins/imageformats
#                -lqgif
#                -lqicns
#                -lqico
#                -lqjp2
#                -ljasper
#                -lqmng
#                -lqtga
#                -lqtiff
#                -lqwbmp
#                -lqwebp
#            -L${QT_STATIC_INSTALL_DIRECTORY}/plugins/egldeviceintegrations
#                -lqeglfs-emu-integration
#                -lqeglfs-kms-egldevice-integration
#                -lqeglfs-kms-integration -lQt5EglFsKmsSupport
#                -lQt5KmsSupport
#                -lqeglfs-x11-integration
#                -lQt5EglFSDeviceIntegration
#                -lQt5EventDispatcherSupport
#                -lQt5ServiceSupport
#                -lQt5ThemeSupport
#                -lQt5FontDatabaseSupport
#                -lQt5FbSupport
#                -lQt5EglSupport
#                -lQt5InputSupport
#                -lQt5PlatformCompositorSupport
#                -lQt5DeviceDiscoverySupport
#            -L${QT_STATIC_INSTALL_DIRECTORY}/plugins/qmltooling
#                -lqmldbg_debugger
#                -lqmldbg_local
#                -lqmldbg_messages
#                -lqmldbg_native
#                -lqmldbg_nativedebugger
#                -lqmldbg_profiler
#                -lqmldbg_quickprofiler
#                -lqmldbg_server
#                -lQt5PacketProtocol
#                -lqmldbg_tcp
#            -L${QT_STATIC_INSTALL_DIRECTORY}/plugins/bearer
#                -lqgenericbearer
#            -L${QT_STATIC_INSTALL_DIRECTORY}/qml/QtQuick.2
#                -lqtquick2plugin
#            -L${QT_STATIC_INSTALL_DIRECTORY}/qml/QtQuick/Window.2
#                -lwindowplugin
#            -L${QT_STATIC_INSTALL_DIRECTORY}/qml/QtQuick/Controls
#                -lqtquickcontrolsplugin
#                -lQt5Widgets
#            -L/home/aobuchowicz/dev/build-qt-static-party/qml/QtQuick/Layouts
#                -lqquicklayoutsplugin
#                -lQt5Quick
#                -lQt5Gui
#                -lqtlibpng
#                -lqtharfbuzz
#                -lQt5Qml
#                -lrt
#                -lQt5Network
                -lQt5Core
                -lm
#                -lqtpcre2
            ${ICU_LIBRARIES}

        -Wl,-Bdynamic
#                -lwayland-egl
#                -lXcomposite
#                -lwayland-client
#                -lwayland-cursor
#                -lXi
#                -lSM
#                -lICE
#                -lmng
#                -ltiff
#                -lwebp
#                -lwebpdemux
#                -lgbm
#                -ldrm
#                -lfontconfig
#                -lfreetype
#                -lXext
#                -lmtdev
#                -linput
#                -lxkbcommon
#                -ludev
#                -lxcb
#                -lX11
#                -lX11-xcb
#                -lts
#                -lEGL

#                -lgthread-2.0
#                -lglib-2.0
#                -lGLESv2
#                -ldl
        )
else(QT_STATIC_INSTALL_DIRECTORY)
    set(QT_LIBRARIES
        Qt5::Core
        )
endif(QT_STATIC_INSTALL_DIRECTORY)

