TEMPLATE = app

QT += core testlib
QT -= gui

CONFIG += c++17 warning_clean exceptions console
CONFIG -= app_bundle
DEFINES += QT_DEPRECATED_WARNINGS QT_ASCII_CAST_WARNINGS QT_USE_QSTRINGBUILDER

include(../qgenerictree.pri)

SOURCES += \
	main.cpp
