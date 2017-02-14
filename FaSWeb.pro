TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += "/home/fas/workspace/cc/C++/QtWorkplace/FaSWeb/include"

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LIBS += -lpthread

SOURCES += main.cpp \
    src/Dispatcher.cpp \
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
    test/thread_test.cpp \
    src/Thread.cpp \
    src/ThreadPool.cpp \
    src/DispatcherThread.cpp \
    src/DispatcherThreadPool.cpp \
    src/Condition.cpp

HEADERS += \
    include/Dispatcher.h \
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
    Buffer.h \
    include/Buffer.h \
    include/Endian.h \
    include/SigIgnore.h \
    include/Types.h \
    include/Log.h \
    include/Thread.h \
    include/ThreadPool.h \
    include/DispatcherThread.h \
    include/DispatcherThreadPool.h \
    include/Condition.h

