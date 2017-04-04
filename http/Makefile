fas_http_static_lib = ../lib/libfashttp.a
fas_http_shared_lib = ../lib/libfashttp.so
fas_utils_static_lib = ../lib/libfasutils.a
fas_utils_shared_lib = ../lib/libfasutils.so
fas_base_static_lib = ../lib/libfas.a
fas_base_shared_lib = ../lib/libfas.so
fas_base_header := $(fas_base)
fas_http_header := $(fas_http)
fas_utils_header := $(fas_utils)
link_libs = -lfasutils -lfas
fas_libs    := ../lib
static_lib_cmd  := ar rcs
shared_lib_cmd  := g++ -shared -fPIC
cflags          := -std=c++11 -Wall 
CC = g++
objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(objects)
	echo $(objects)
	$(shared_lib_cmd) $^ -o $(fas_http_shared_lib)
	$(static_lib_cmd) $(fas_http_static_lib) $^ 

%.o:%.cpp
	$(CC) -c $^ -I${fas_base_header} -I${fas_utils_header} -I${fas_http_header} $(cflags) -o $@ -L$(fas_libs) $(link_libs) -static

.PHONY : clean
clean:
	rm *.o -rf
