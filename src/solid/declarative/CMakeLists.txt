ecm_add_qml_module(soliddeclarative URI "org.kde.solid" DEPENDENCIES QtQuick)

target_sources(soliddeclarative PRIVATE
  devices.cpp
)

target_link_libraries(soliddeclarative PRIVATE
  Qt::Core
  Qt::Qml
  KF6::Solid
)

ecm_finalize_qml_module(soliddeclarative DESTINATION ${KDE_INSTALL_QMLDIR})

