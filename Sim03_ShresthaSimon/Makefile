TARGET = Sim03
LIBS = -lm
HEADERS = Config.h MetaData.h #List of all header files
SRCS = main.cpp Config.cpp MetaData.cpp #List of all source files
OBJECTS := $(patsubst %.cpp,%.o,$(SRCS))
CXX = g++ -pthread
CXX_FLAGS = -Wall -std=c++11 -g


.PHONY: default all clean-


all: $(TARGET)


%.o: %.cpp $(HEADERS)
	$(CXX) $(CXX_FLAGS) -c $< -o $@


$(TARGET): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) $(LIBS) -o $@


clean:
	-rm -f *.o
	-rm -f $(TARGET)