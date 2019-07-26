#-------------------------------------------------
#
# Project created by QtCreator 2017-08-03T15:13:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisualMicrosaccadesExplorer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include \
               include/plots \
               include/IO \
               include/saccadeProcessing \
               include/vis \
               include/vis/timeline \
               include/vis/stimulus \
               include/eyeTrackingData

SOURCES+= $$PWD/src/*.cpp \
          $$PWD/src/plots/*.cpp \
          $$PWD/src/IO/*.cpp \
          $$PWD/src/saccadeProcessing/*.cpp \
          $$PWD/src/vis/*.cpp \
          $$PWD/src/vis/timeline/*.cpp \
          $$PWD/src/vis/stimulus/*.cpp \
          $$PWD/src/eyeTrackingData/*.cpp

HEADERS+= $$PWD/include/*.h \
          $$PWD/include/plots/*.h \
          $$PWD/include/IO/*.h \
          $$PWD/include/saccadeProcessing/*.h \
          $$PWD/include/vis/*.h \
          $$PWD/include/vis/timeline/*.h \
          $$PWD/include/vis/stimulus/*.h \
          $$PWD/include/eyeTrackingData/*.h

RESOURCES = resources/darkstyle.qrc \
            resources/resources.qrc

DISTFILES +=
