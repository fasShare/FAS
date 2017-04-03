libs := -lfashttp -lfasutils -lfas -lpthread
cflags := -std=c++11 -static -Wall
CC = g++

all:
	echo $(fas_base)
	echo $(fas_libs)
	echo $(fas_test_bin)
	
	$(CC) TcpServerTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/TcpServerTest $(libs)
	$(CC) HttpServerTest.cpp  -I${fas_base} -I${fas_http} -L${fas_libs} $(cflags) -o $(fas_test_bin)/HttpServerTest $(libs)
#	$(CC) ThreadLoggerTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/ThreadLoggerTest $(libs)
#	$(CC) EventHandleClientTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/EventHandleClientTest $(libs)
#	$(CC) LogTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/LogTest $(libs)
#	$(CC) TimerHeapTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/TimerHeapTest $(libs)
#	$(CC) TimersSchedulerTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/TimersSchedulerTest $(libs)
#	$(CC) EventFdTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/EventFdTest $(libs)


.PHONY : clean
clean:
	rm *.o   -rf 
