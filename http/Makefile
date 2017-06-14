FAS_HTTP_STATIC_LIB							:= $(FAS_LIBS_DIR)/libfashttp.a
FAS_HTTP_SHARED_LIB							:= $(FAS_LIBS_DIR)/libfashttp.so

OBJECTS											:= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OBJECTS)
	$(SHARED_LIB_CMD) $^ -L$(FAS_LIBS_DIR) -o $(FAS_HTTP_SHARED_LIB)
	$(STATIC_LIB_CMD) $(FAS_HTTP_STATIC_LIB) $^ 

%.o:%.cpp $(HTTP_HEADERS) 
	$(CC) -c $< $(HTTP_INCLUDE_DIR) $(CFLAG) -o $@ $(HTTP_LIBS) 

.PHONY : clean
clean:
	rm *.o -rf
