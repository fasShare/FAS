FAS_STATIC_LIB							:= $(FAS_LIBS_DIR)/libfas.a
FAS_SHARED_LIB							:= $(FAS_LIBS_DIR)/libfas.so

OBJECTS											:= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OBJECTS)
	$(SHARED_LIB_CMD) $^ -L$(FAS_LIBS_DIR) $(THIRD_LIB_DIR)  -o $(FAS_SHARED_LIB)
	$(STATIC_LIB_CMD) $(FAS_STATIC_LIB) $^  

%.o:%.cpp $(BASE_HEADERS)
	$(CC) -c $< ${BASE_INCLUDE_DIR} $(CFLAG) -o $@ $(BASE_LIBS) 

.PHONY : clean
clean:
	rm *.o -rf
