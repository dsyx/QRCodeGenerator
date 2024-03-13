QT       += core gui printsupport serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(qzxing/src/qzxing.pri)

SOURCES += \
    MenuBar.cpp \
    OperationWidget.cpp \
    PrinterSetupDialog.cpp \
    QRCodeGenerator.cpp \
    QRCodeSetupWidget.cpp \
    SerialSetupDialog.cpp \
    main.cpp

HEADERS += \
    MenuBar.h \
    OperationWidget.h \
    PrinterSetupDialog.h \
    QRCodeGenerator.h \
    QRCodeSetupWidget.h \
    SerialSetupDialog.h

TRANSLATIONS += \
    QRCodeGenerator_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
