HEADERS += \
	$$PWD/qgenerictreebase.h

INCLUDEPATH += $$PWD

QDEP_PACKAGE_EXPORTS += Q_GENERIC_TREE_EXPORT
!qdep_build: DEFINES += "Q_GENERIC_TREE_EXPORT="
