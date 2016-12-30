TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    src/Epoll.cpp \
    src/NetAddress.cpp \
    src/Socket.cpp \
    src/Executor.cpp \
    src/Dispatcher.cpp \
    src/Mutex.cpp \
    src/MutexLocker.cpp \
    src/TcpServer.cpp \
    src/Events.cpp \
    src/EventsPoller.cpp

HEADERS += \
    include/Epoll.h \
    include/NetAddress.h \
    include/Socket.h \
    include/Executor.h \
    include/Dispatcher.h \
    include/Mutex.h \
    include/MutexLocker.h \
    include/NetBaseTypes.h \
    include/TcpServer.h \
    include/Events.h \
    include/EventsPoller.h

DISTFILES += \
    Makefile

