TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += "/home/fas/workspace/cc/C++/QtWorkplace/FaSWeb/include"

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    src/Dispatcher.cpp \
    src/Epoll.cpp \
    src/Events.cpp \
    src/HttpResponse.cpp \
    src/Mutex.cpp \
    src/MutexLocker.cpp \
    src/NetAddress.cpp \
    src/Poll.cpp \
    src/Socket.cpp \
    src/TcpServer.cpp \
    src/Handle.cpp \
    src/Poller.cpp

HEADERS += \
    include/Dispatcher.h \
    include/Epoll.h \
    include/Events.h \
    include/HttpResponse.h \
    include/Mutex.h \
    include/MutexLocker.h \
    include/NetAddress.h \
    include/NetBaseTypes.h \
    include/Poll.h \
    include/Socket.h \
    include/TcpServer.h \
    include/Handle.h \
    include/Poller.h

