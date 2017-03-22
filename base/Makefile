fas_static_lib = ../lib/libfas.a
fas_shared_lib = ../lib/libfas.so
fas_base_header := $(fas_base)
fas_libs    := ../lib
static_lib_cmd  := ar rcs
shared_lib_cmd  := g++ -shared -fPIC
cflags          := -std=c++11 -Wall 
CC = g++
objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(objects)
	echo $(objects)
	$(shared_lib_cmd) $^ -o $(fas_shared_lib)
	$(static_lib_cmd) $(fas_static_lib) $^ 

%.o:%.cpp
	$(CC) -c $^ -I${fas_base_header} $(cflags) -o $@

.PHONY : clean
clean:
	rm *.o -rf
