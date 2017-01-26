APPNAME = APP
CURDIR = $(shell pwd)
export NETBASE_HEADERS  := $(CURDIR)/include
export NETBASE_SRCS	    := $(CURDIR)/src
export NETBASE_LIBS     := $(CURDIR)/lib
libs :=  -lNetBase -lpthread
cflags := -std=c++11 -static -Wall
CC = g++

objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:
	echo $(CURDIR)
	echo $(NETBASE_HEADERS)
	echo $(NETBASE_SRCS)
	echo $(NETBASE_LIBS)

	make -C src
	make $(APPNAME)
	make -C test

$(APPNAME):$(objects)
	echo $(objects)
	$(CC) $^ -L${NETBASE_LIBS} $(cflags) -o $@ $(libs) 

%.o:%.cpp
	echo $(objects)
	$(CC) -c $^ -I${NETBASE_HEADERS} -L${NETBASE_LIBS} $(cflags) -o $@

.PHONY : clean
clean:
	rm *.o $(APPNAME) ./lib/*  -rf 
	make clean -C src
