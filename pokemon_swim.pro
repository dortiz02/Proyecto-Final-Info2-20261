QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += src/headers

SOURCES += \
    src/main.cpp \
    src/Personaje.cpp \
    src/Jugador.cpp \
    src/AgenteRival.cpp \
    src/Obstaculo.cpp \
    src/Nivel.cpp \
    src/Juego.cpp \
    src/MainWindow.cpp \
    src/NivelWidget.cpp

HEADERS += \
    src/headers/Personaje.h \
    src/headers/Jugador.h \
    src/headers/AgenteRival.h \
    src/headers/Obstaculo.h \
    src/headers/Nivel.h \
    src/headers/Juego.h \
    src/headers/MainWindow.h \
    src/headers/NivelWidget.h

RESOURCES += assets/recursos.qrc

TARGET   = pokemon_swim
TEMPLATE = app
