libs :=  -lNetBase -lpthread
cflags := -std=c++11 -static -Wall
CC = g++

all:
	echo $(NETBASE_HEADERS)
	echo $(NETBASE_LIBS)
	
	$(CC) thread_test.cpp  -I${NETBASE_HEADERS} -L${NETBASE_LIBS} $(cflags) -o thread_test $(libs)


.PHONY : clean
clean:
	rm *.o   -rf 
