TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ./base

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    base/Buffer.cpp \
    base/Condition.cpp \
    base/Epoll.cpp \
    base/EventLoop.cpp \
    base/EventLoopThread.cpp \
    base/EventLoopThreadPool.cpp \
    base/Events.cpp \
    base/Handle.cpp \
    base/Log.cpp \
    base/Mutex.cpp \
    base/MutexLocker.cpp \
    base/NetAddress.cpp \
    base/Poll.cpp \
    base/Poller.cpp \
    base/SigIgnore.cpp \
    base/Socket.cpp \
    base/TcpConnection.cpp \
    base/TcpServer.cpp \
    base/Thread.cpp \
    base/ThreadPool.cpp \
    base/Timer.cpp \
    base/TimerHeap.cpp \
    base/TimersScheduler.cpp \
    base/Timestamp.cpp \
    test/EventFdTest.cpp \
    test/EventHandleClientTest.cpp \
    test/LogTest.cpp \
    test/TcpServerTest.cpp \
    test/ThreadLoggerTest.cpp \
    test/TimerHeapTest.cpp \
    test/TimersSchedulerTest.cpp

HEADERS += \
    base/Buffer.h \
    base/Condition.h \
    base/Endian.h \
    base/Epoll.h \
    base/EventLoop.h \
    base/EventLoopThread.h \
    base/EventLoopThreadPool.h \
    base/Events.h \
    base/Handle.h \
    base/Log.h \
    base/Mutex.h \
    base/MutexLocker.h \
    base/NetAddress.h \
    base/Poll.h \
    base/Poller.h \
    base/SigIgnore.h \
    base/Socket.h \
    base/TcpConnection.h \
    base/TcpServer.h \
    base/Thread.h \
    base/ThreadPool.h \
    base/Timer.h \
    base/TimerHeap.h \
    base/TimersScheduler.h \
    base/Timestamp.h \
    base/Types.h

