EXEC	= move
CXX	= g++
MAIN	= mover.cpp
OPTIONS	= -O3 -Wall
LIBS	= -lufrn_lynx -lm `pkg-config --libs opencv`


main:	clean
	$(CXX) $(CFLAGS) $(OPTIONS) $(MAIN) -I./include/ -L./lib/ $(LIBS) -o $(EXEC)

clean:
	clear
	rm -f include/*~
	rm -f lib/*~
	rm -f *~

