APPNAME = NetBase
MUDUO_HEADERS_DIR := ../../build/
MUDUO_LIBS_DIR	:= ../../build/release/lib/
libs := -lmuduo_base -lpthread
cflags := -march=native -static -std=c++11
CC = g++


objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))


$(APPNAME):$(objects)
	echo $(objects)
	$(CC) $^ -I${MUDUO_HEADERS_DIR} -L${MUDUO_LIBS_DIR} $(cflags) -o $@ $(libs)
%.o:%.cpp
	echo $(objects)
	$(CC) -c $^ -I$(MUDUO_HEADERS_DIR) -L$(MUDUO_LIBS_DIR) $(cflags) -o $@

.PHONY : clean
clean:
	-rm *.o -rf
