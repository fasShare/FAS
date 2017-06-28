FAS_MDM_STATIC_LIB							:= $(FAS_LIBS_DIR)/libfasmdm.a

OBJECTS											:= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OBJECTS)
	$(STATIC_LIB_CMD) $(FAS_MDM_STATIC_LIB) $^ 

%.o:%.cpp $(MDM_HEADERS) 
	$(CC) -c $< $(MDM_INCLUDE_DIR) $(CFLAG) -o $@ $(MDM_LIBS) 

.PHONY : clean
clean:
	rm *.o -rf
