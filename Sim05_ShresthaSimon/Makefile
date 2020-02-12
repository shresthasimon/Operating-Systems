TARGET = Sim05
LIBS = -lm
HEADERS = config.h metadata.h OperatingSystem.h resourceManager.h #List of all header files
SRCS =  config.cpp metadata.cpp OperatingSystem.cpp resourceManager.cpp main.cpp#List of all source files
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