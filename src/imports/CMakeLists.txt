find_package(Qt${QT_MAJOR_VERSION}Qml ${REQUIRED_QT_VERSION} CONFIG QUIET)

set_package_properties(Qt${QT_MAJOR_VERSION}Qml PROPERTIES  URL "git://gitorious.org/qt/qtdeclarative.git"
                                          DESCRIPTION "QML support for Solid"
                                          TYPE OPTIONAL
                                          PURPOSE "Allows to use Solid in QML code"
                         )


if(NOT TARGET Qt${QT_MAJOR_VERSION}::Qml)
    message(STATUS "Qt${QT_MAJOR_VERSION}Qml not found, qml imports will not be built.")
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
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Qml
)

install(TARGETS solidextensionplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/solid)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/solid)

