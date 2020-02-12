add_subdirectory(solid)
add_subdirectory(tools)
add_subdirectory(imports)

ecm_qt_install_logging_categories(
    EXPORT SOLID
    FILE solid.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
