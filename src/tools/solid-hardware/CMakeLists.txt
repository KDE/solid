add_executable(solid-hardware solid-hardware.cpp)
ecm_mark_nongui_executable(solid-hardware)
target_link_libraries(solid-hardware ${KDE4_KDECORE_LIBS} ${KDE4_SOLID_LIBS})
install(TARGETS solid-hardware ${INSTALL_TARGETS_DEFAULT_ARGS})



