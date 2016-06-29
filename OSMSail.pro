TEMPLATE = app

QT_CONFIG -= no-pkg-config
CONFIG += qt warn_on link_pkgconfig thread c++11 silent sailfishapp
CONFIG += release
CONFIG -= debug

CONFIG(debug, debug|release) {
QMAKE_FLAGS += -std=c++11 -O2 -pthread -fopenmp -mmmx -msse -msse2 -mssse3 -msse4.1
QMAKE_CFLAGS += -O2 -pthread -fopenmp -mmmx -msse -msse2 -mssse3 -msse4.1
QMAKE_CXXFLAGS += -std=c++11 -O2 -pthread -fopenmp -mmmx -msse -msse2 -mssse3 -msse4.1
}
CONFIG(release, debug|release) {
QMAKE_FLAGS += -std=c++11 -O2 -pthread -fopenmp 
QMAKE_CFLAGS += -O2 -pthread -fopenmp 
QMAKE_CXXFLAGS += -std=c++11 -O2 -pthread -fopenmp 
}

sailfishapp: DEFINES += _SAILFISH_BUILD

release:DESTDIR = release
release:LIBDIR = $$PWD/libs/release
debug:DESTDIR = debug
debug:LIBDIR = $$PWD/libs/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rrc
UI_DIR = $$DESTDIR/.ui

## PART FOR COMPILING libosmscout

CONFIG(release, debug|release) {
  message( "Building libraries for release" )
  libosmscout_lib = -fopenmp -L$$LIBDIR -Wl,--whole-archive -losmscoutmapqt -losmscoutmap -fopenmp -losmscout -lpthread  -Wl,--no-whole-archive
  libosmscout.commands = $$PWD/build_libosmscout.sh release
}
CONFIG(debug, debug|release) {
  message( "Building libraries for debug" )
  libosmscout_lib = -fopenmp -mmmx -msse -msse2 -mssse3 -msse4.1 -L$$LIBDIR -Wl,--whole-archive -losmscoutmapqt -losmscoutmap -fopenmp -losmscout -lpthread  -Wl,--no-whole-archive
  libosmscout.commands = $$PWD/build_libosmscout.sh debug
}
LIBS += $$libosmscout_lib 
libosmscout.target = libosmscout_lib
QMAKE_EXTRA_TARGETS += libosmscout
PRE_TARGETDEPS += libosmscout_lib

## END OF libosmscout COMPILATION

QT += core gui widgets qml quick

qtHaveModule(svg) {
 QT += svg
}

qtHaveModule(positioning) {
 QT += positioning
}


#PKGCONFIG += libosmscout-map-qt

gcc:QMAKE_CXXFLAGS += -fopenmp -pthread

INCLUDEPATH = src
INCLUDEPATH += $$PWD/libosmscout/include
INCLUDEPATH += $$PWD/libosmscout-map/include
INCLUDEPATH += $$PWD/libosmscout-map-qt/include


SOURCES = src/OSMSail.cpp \
          src/Settings.cpp \
          src/Theme.cpp \
          src/DBThread.cpp \
          src/MapWidget.cpp \
          src/SearchLocationModel.cpp \
          src/RoutingModel.cpp

HEADERS = src/Settings.h \
          src/Theme.h \
          src/DBThread.h \
          src/MapWidget.h \
          src/SearchLocationModel.h \
          src/RoutingModel.h

DISTFILES += \
    qml/custom/MapButton.qml \
    qml/Main.qml \
    qml/custom/LineEdit.qml \
    qml/custom/DialogActionButton.qml \
    qml/custom/LocationSearch.qml \
    qml/custom/ScrollIndicator.qml \
    qml/custom/MapDialog.qml \
    qml/AboutDialog.qml \
    qml/SearchDialog.qml \
    pics/DeleteText.svg \
    pics/Minus.svg \
    pics/Plus.svg \
    pics/Search.svg

RESOURCES += \
    res.qrc

