add_executable(solid-hardware5 solid-hardware.cpp)
ecm_mark_nongui_executable(solid-hardware5)
target_link_libraries(solid-hardware5 KF5::Solid)
install(TARGETS solid-hardware5 ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
