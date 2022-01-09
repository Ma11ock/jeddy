CPP=g++
CPPFLAGS=-std=c++17 -c

all: source.o main.o
	$(CPP) $^ -lncurses -lm -o jeddy

source.o: main.cpp
	echo '#include "source.hpp"' > source.cpp 
	xxd -i $< >> source.cpp
	$(CPP) $(CPPFLAGS) source.cpp -o $@

main.o: main.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

clean:
	rm source.cpp *.o
