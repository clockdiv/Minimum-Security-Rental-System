QT       += core gui sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    InventoryWidget.cpp \
    InventoryWidgetSmall.cpp \
    RentalWidget.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp

HEADERS += \
    InventoryWidget.h \
    InventoryWidgetSmall.h \
    RentalWidget.h \
    mainwindow.h \
    settings.h

FORMS += \
    InventoryWidget.ui \
    InventoryWidgetSmall.ui \
    RentalWidget.ui \
    mainwindow.ui \
    settings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target