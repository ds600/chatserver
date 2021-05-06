TEMPLATE     = app
TARGET       = chatserver
INCLUDEPATH += .
QT          += widgets network

# Input
SOURCES     += main.cpp mainform.cpp client.cpp
HEADERS     += config.h mainform.h   client.h
FORMS       +=          mainform.ui
