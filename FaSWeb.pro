TEMPLATE = app
CONFIG += console
INCLUDEPATH += "/home/fas/workspace/cc/C++/QtWorkplace/FaSWeb/include/"
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    src/Dispatcher.cpp \
    src/Epoll.cpp \
    src/Events.cpp \
    src/EventsPoller.cpp \
    src/Executor.cpp \
    src/HttpResponse.cpp \
    src/Mutex.cpp \
    src/MutexLocker.cpp \
    src/NetAddress.cpp \
    src/Poll.cpp \
    src/Socket.cpp \
    src/TcpServer.cpp

HEADERS += \
    include/Dispatcher.h \
    include/Epoll.h \
    include/Events.h \
    include/EventsPoller.h \
    include/Executor.h \
    include/HttpResponse.h \
    include/Mutex.h \
    include/MutexLocker.h \
    include/NetAddress.h \
    include/NetBaseTypes.h \
    include/Poll.h \
    include/Socket.h \
    include/TcpServer.h

