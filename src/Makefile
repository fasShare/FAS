NETBASE_STATIC_LIB = ../lib/libNetBase.a
NETBASE_SHARED_LIB = ../lib/libNetBase.so
NETBASE_HEADERS := ../include
NETBASE_LIBS    := ../lib
STATIC_LIB_CMD  := ar rcs
SHARED_LIB_CMD  := g++ -shared -fPIC
cflags          := -std=c++11 -Wall 
CC = g++
objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(objects)
	echo $(objects)
	$(SHARED_LIB_CMD) $^ -o $(NETBASE_SHARED_LIB)
	$(STATIC_LIB_CMD) $(NETBASE_STATIC_LIB) $^ 
#	cp $(NETBASE_SHARED_LIB) $(NETBASE_STATIC_LIB) $(NETBASE_LIBS)	

%.o:%.cpp
	$(CC) -c $^ -I${NETBASE_HEADERS} $(cflags) -o $@

.PHONY : clean
clean:
	rm *.o -rf
