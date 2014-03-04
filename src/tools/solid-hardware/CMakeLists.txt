add_executable(solid-hardware solid-hardware.cpp)
ecm_mark_nongui_executable(solid-hardware)
target_link_libraries(solid-hardware KF5::Solid KF5::KDE4Support)
install(TARGETS solid-hardware ${INSTALL_TARGETS_DEFAULT_ARGS})



