# Makefile for DYNAMO command line subgraph search
# Patrick Mackey, 2018

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
CFLAGS = --std=c++11 -O2
INCLUDES =
LDFLAGS = 
TARGET = graph_search

# Compiler (Must be g++ 4.9 or greater)
#CXX = g++-4.9  
CXX = g++ 

.SUFFIXES:
.SUFFIXES: .o .cpp

.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
