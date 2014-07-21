CC = g++

CFLAGS = -g -Wall -pedantic -std=c++11
WEKA = -I include/wekacpp/include -I include/wekacpp/lib
INCLUDES = -I PDA -I ML -I modelClasses -I dataAcquisition $(WEKA)


all:
	$(CC) $(CFLAGS) main.cpp $(INCLUDES) -o a.out

clean:
	$(RM) main.o
