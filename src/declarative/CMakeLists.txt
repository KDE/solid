ecm_add_qml_module(solidextensionplugin URI "org.kde.solid" DEPENDENCIES QtQuick)

target_sources(solidextensionplugin PRIVATE
  devices.cpp
  solidextensionplugin.cpp
)

target_link_libraries(solidextensionplugin PRIVATE
  Qt::Core
  Qt::Qml
  KF6::Solid
)

ecm_finalize_qml_module(solidextensionplugin DESTINATION ${KDE_INSTALL_QMLDIR})

# todo add docs
# ecm_generate_qdoc(sonnetquickplugin sonnetquick.qdocconf)
