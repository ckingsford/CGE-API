CC = g++

CFLAGS = -g -Wall -pedantic -std=c++11
INCLUDES = -I PDA -I dataAcquisition -I ML -I modelClasses -I utility 
DEPENDENCIES = $(wildcard PDA/*.cpp dataAcquisition/*.cpp ML/*.cpp modelClasses/*.cpp utility/*.cpp)

all: CGE

CGE: main.cpp $(HEADERS)
	$(CC) $(CFLAGS) main.cpp $(DEPENDENCIES) $(INCLUDES) -o a.out

clean:
	$(RM) *.o
