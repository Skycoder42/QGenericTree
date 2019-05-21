HEADERS += \
	$$PWD/qgenerictreebase.h \
	$$PWD/qorderedtree.h \
	$$PWD/qunorderedtree.h

INCLUDEPATH += $$PWD

QDEP_PACKAGE_EXPORTS += Q_GENERIC_TREE_EXPORT
!qdep_build: DEFINES += "Q_GENERIC_TREE_EXPORT="
