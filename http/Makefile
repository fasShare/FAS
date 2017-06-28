FAS_HTTP_STATIC_LIB							:= $(FAS_LIBS_DIR)/libfashttp.a

OBJECTS											:= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OBJECTS)
	$(STATIC_LIB_CMD) $(FAS_HTTP_STATIC_LIB) $^ 

%.o:%.cpp $(HTTP_HEADERS) 
	$(CC) -c $< $(HTTP_INCLUDE_DIR) $(CFLAG) -o $@ $(HTTP_LIBS) 

.PHONY : clean
clean:
	rm *.o -rf
