FAS_STATIC_LIB							:= $(FAS_LIBS_DIR)/libfas.a

OBJECTS											:= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OBJECTS)
	$(STATIC_LIB_CMD) $(FAS_STATIC_LIB) $^  

%.o:%.cpp $(BASE_HEADERS)
	$(CC) -c $< ${BASE_INCLUDE_DIR} $(CFLAG) -o $@ $(BASE_LIBS) 

.PHONY : clean
clean:
	rm *.o -rf
