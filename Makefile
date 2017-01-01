APPNAME = TcpSer
NETBASE_HEADERS := ./include
NETBASE_SRCS	:= ./src
NETBASE_LIBS    := ./lib
libs :=  -lNetBase -lpthread
cflags := -std=c++11 -static
CC = g++

objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:
	make -C src
	make clean -C src
	make $(APPNAME)

$(APPNAME):$(objects)
	echo $(objects)
	$(CC) $^ -I${NETBASE_HEADERS} -L${NETBASE_LIBS} $(cflags) -o $@ $(libs) 

%.o:%.cpp
	echo $(objects)
	$(CC) -c $^ -I${NETBASE_HEADERS} -L${NETBASE_LIBS} $(cflags) -o $@

.PHONY : clean
clean:
	rm *.o $(APPNAME) -rf 

