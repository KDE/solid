find_package(Qt6Qml ${REQUIRED_QT_VERSION} CONFIG QUIET)

set_package_properties(Qt6Qml PROPERTIES  URL "git://gitorious.org/qt/qtdeclarative.git"
                                          DESCRIPTION "QML support for Solid"
                                          TYPE OPTIONAL
                                          PURPOSE "Allows to use Solid in QML code"
                         )


if(NOT TARGET Qt6::Qml)
    message(STATUS "Qt6Qml not found, qml imports will not be built.")
    return()
endif()

add_library(solidextensionplugin SHARED
    solidextensionplugin.cpp
    devices.cpp
)

target_link_libraries(solidextensionplugin
    KF6::Solid
    Qt6::Core
    Qt6::Qml
)

if(MINGW)
    set_target_properties(solidextensionplugin PROPERTIES PREFIX "")
endif()

install(TARGETS solidextensionplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/solid)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/solid)

