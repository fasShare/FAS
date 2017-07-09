StaticLibrary('./lib/fasutils',
        Glob("utils/*.cpp"),
        CPPPATH=['.', 'utils'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
        )
StaticLibrary('./lib/fas',
        Glob("base/*.cpp"),
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
        )
StaticLibrary('./lib/fashttp',
        Glob("http/*.cpp"),
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base', 'http'],
        LIBS = ['fasutils', 'fas', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
        )
StaticLibrary('./lib/fasmdm',
        Glob("mdm/*.cpp"),
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base', 'mdm'],
        LIBS = ['fasutils', 'fas', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
        )

Program('bin/TcpServer', 'test/TcpServerTest.cpp',
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'fas', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
        )
Program('bin/JsonTest', 'test/JsonTest.cpp',
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'fas', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
        )
