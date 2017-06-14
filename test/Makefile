all:
	$(CC) TcpServerTest.cpp  $(TEST_INCLUDE_DIR) -L$(FAS_LIBS_DIR) $(CFLAG) -o $(FAS_TEST_BIN)/TcpServerTest $(TEST_BIN_LIBS)
	$(CC) HttpServerTest.cpp  $(TEST_INCLUDE_DIR) -L$(FAS_LIBS_DIR) $(CFLAG) -o $(FAS_TEST_BIN)/HttpServerTest $(TEST_BIN_LIBS)
#	$(CC) ThreadLoggerTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/ThreadLoggerTest $(libs)
#	$(CC) EventHandleClientTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/EventHandleClientTest $(libs)
#	$(CC) LogTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/LogTest $(libs)
#	$(CC) TimerHeapTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/TimerHeapTest $(libs)
#	$(CC) TimersSchedulerTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/TimersSchedulerTest $(libs)
#	$(CC) EventFdTest.cpp  -I${fas_base} -L${fas_libs} $(cflags) -o $(fas_test_bin)/EventFdTest $(libs)


.PHONY : clean
clean:
	rm *.o   -rf 
