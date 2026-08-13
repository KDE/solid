set(backend_sources
    kiofusemanager.cpp
    kiofusemanager.h
    kiofusedevice.cpp
    kiofusedevice.h
    kiofusestorageaccess.cpp
    kiofusestorageaccess.h
    kiofusenetworkshare.cpp
    kiofusenetworkshare.h
    kiofuseutils.cpp
    kiofuseutils.h
)

ecm_qt_declare_logging_category(backend_sources
    HEADER kiofuse_debug.h
    IDENTIFIER Solid::Backends::KioFuse::KIOFUSE_LOG
    CATEGORY_NAME kf.solid.backends.kiofuse
    DESCRIPTION "KIO FUSE (Solid)"
    EXPORT SOLID
)
