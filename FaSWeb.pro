TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += "/home/fas/workspace/cc/C++/QtWorkplace/FaSWeb/include"

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LIBS += -lpthread

SOURCES += main.cpp \
    src/Epoll.cpp \
    src/Events.cpp \
    src/Mutex.cpp \
    src/MutexLocker.cpp \
    src/NetAddress.cpp \
    src/Poll.cpp \
    src/Socket.cpp \
    src/TcpServer.cpp \
    src/Handle.cpp \
    src/Poller.cpp \
    src/Timestamp.cpp \
    src/Buffer.cpp \
    src/SigIgnore.cpp \
    src/Log.cpp \
    src/Thread.cpp \
    src/ThreadPool.cpp \
    src/Condition.cpp \
    src/EventLoop.cpp \
    src/EventLoopThread.cpp \
    src/EventLoopThreadPool.cpp \
    src/TcpConnection.cpp

HEADERS += \
    include/Epoll.h \
    include/Events.h \
    include/Mutex.h \
    include/MutexLocker.h \
    include/NetAddress.h \
    include/Poll.h \
    include/Socket.h \
    include/TcpServer.h \
    include/Handle.h \
    include/Poller.h \
    include/Timestamp.h \
    include/Buffer.h \
    include/Endian.h \
    include/SigIgnore.h \
    include/Log.h \
    include/Thread.h \
    include/ThreadPool.h \
    include/Condition.h \
    include/EventLoop.h \
    include/EventLoopThread.h \
    include/EventLoopThreadPool.h \
    include/Default.h \
    include/TcpConnection.h

