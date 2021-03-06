QT += core quick quickwidgets

CONFIG += c++17
CONFIG(debug, debug|release) {
    CONFIG += qml_debug
}

INCLUDEPATH += libs/desktopentry/include

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\'/../lib'

LIBS += -lX11 -lxcb
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/libs/desktopentry/target/debug
} else {
    LIBS += -L$$PWD/libs/desktopentry/target/release
}
LIBS += -ldesktopentry

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ConfigFile.h \
    DesktopEntry.h \
    Dock.h \
    DockWidget.h \
    Ewmh.h \
    IconImageProvider.h \
    Item.h \
    MenuWidget.h \
    PopUpManager.h \
    ToolTipWidget.h

SOURCES += \
    ConfigFile.cpp \
    DesktopEntry.cpp \
    Dock.cpp \
    DockWidget.cpp \
    Ewmh.cpp \
    IconImageProvider.cpp \
    Item.cpp \
    MenuWidget.cpp \
    PopUpManager.cpp \
    ToolTipWidget.cpp \
    main.cpp

RESOURCES += resources/resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

