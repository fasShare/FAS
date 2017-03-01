FAS_STATIC_LIB = ../lib/libfas.a
FAS_SHARED_LIB = ../lib/libfas.so
FAS_HEADERS := ../include
FAS_LIBS    := ../lib
STATIC_LIB_CMD  := ar rcs
SHARED_LIB_CMD  := g++ -shared -fPIC
cflags          := -std=c++11 -Wall 
CC = g++
objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(objects)
	echo $(objects)
	$(SHARED_LIB_CMD) $^ -o $(FAS_SHARED_LIB)
	$(STATIC_LIB_CMD) $(FAS_STATIC_LIB) $^ 
#	cp $(NETBASE_SHARED_LIB) $(NETBASE_STATIC_LIB) $(NETBASE_LIBS)	

%.o:%.cpp
	$(CC) -c $^ -I${FAS_HEADERS} $(cflags) -o $@

.PHONY : clean
clean:
	rm *.o -rf
