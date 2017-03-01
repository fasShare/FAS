libs :=  -lfas -lpthread
cflags := -std=c++11 -static -Wall
CC = g++

all:
	echo $(FAS_HEADERS)
	echo $(FAS_LIBS)
	echo $(TEST_BIN)
	
	$(CC) TcpServerTest.cpp  -I${FAS_HEADERS} -L${FAS_LIBS} $(cflags) -o $(TEST_BIN)/TcpServerTest $(libs)
	$(CC) TimerHeapTest.cpp  -I${FAS_HEADERS} -L${FAS_LIBS} $(cflags) -o $(TEST_BIN)/TimerHeapTest $(libs)
	$(CC) TimersSchedulerTest.cpp  -I${FAS_HEADERS} -L${FAS_LIBS} $(cflags) -o $(TEST_BIN)/TimersSchedulerTest $(libs)
	$(CC) EventFdTest.cpp  -I${FAS_HEADERS} -L${FAS_LIBS} $(cflags) -o $(TEST_BIN)/EventFdTest $(libs)


.PHONY : clean
clean:
	rm *.o   -rf 
