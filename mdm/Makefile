FAS_MDM_STATIC_LIB							:= $(FAS_LIBS_DIR)/libfasmdm.a
FAS_MDM_SHARED_LIB							:= $(FAS_LIBS_DIR)/libfasmdm.so

OBJECTS											:= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OBJECTS)
	$(SHARED_LIB_CMD) $^ -L$(FAS_LIBS_DIR) -o $(FAS_MDM_SHARED_LIB)
	$(STATIC_LIB_CMD) $(FAS_MDM_STATIC_LIB) $^ 

%.o:%.cpp $(MDM_HEADERS) 
	$(CC) -c $< $(MDM_INCLUDE_DIR) $(CFLAG) -o $@ $(MDM_LIBS) 

.PHONY : clean
clean:
	rm *.o -rf
