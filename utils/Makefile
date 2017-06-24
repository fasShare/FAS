FAS_UTILS_STATIC_LIB							:= $(FAS_LIBS_DIR)/libfasutils.a
FAS_UTILS_SHARED_LIB							:= $(FAS_LIBS_DIR)/libfasutils.so

OBJECTS											:= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OBJECTS)
	$(SHARED_LIB_CMD) $^ -L$(FAS_LIBS_DIR) -o $(FAS_UTILS_SHARED_LIB)
	$(STATIC_LIB_CMD) $(FAS_UTILS_STATIC_LIB) $^ 

%.o:%.cpp $(UTILS_HEADERS) 
	$(CC) -c $< $(UTILS_INCLUDE_DIR) $(CFLAG) -o $@ $(UTILS_LIBS) 

.PHONY : clean
clean:
	rm *.o -rf
