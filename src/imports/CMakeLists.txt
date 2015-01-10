find_package(Qt5Qml ${REQUIRED_QT_VERSION} CONFIG QUIET)

set_package_properties(Qt5Qml PROPERTIES  URL "git://gitorious.org/qt/qtdeclarative.git"
                                          DESCRIPTION "QML support for Solid"
                                          TYPE OPTIONAL
                                          PURPOSE "Allows to use Solid in QML code"
                         )


if(NOT Qt5Qml_FOUND)
    message(STATUS "Qt5Qml not found, qml imports will not be built.")
    return()
endif()

set(solidextensionplugin_SRCS
    ${solid_common_SRCS}
    solidextensionplugin.cpp
    devices.cpp
    )

add_library(solidextensionplugin SHARED ${solidextensionplugin_SRCS})

target_link_libraries(
    solidextensionplugin
    KF5::Solid
    Qt5::Core
    Qt5::Qml
)

install(TARGETS solidextensionplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/solid)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/solid)

