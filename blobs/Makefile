ALL = blobs
CXX = clang++
FLAGS = -g -ferror-limit=5 -I/usr/local/include
LIBS = -L/usr/local/lib -framework OpenGL -framework Cocoa -framework GLUT

all: $(ALL)

amc.o: amc.h amc.cpp
	$(CXX) -o amc.o $(FLAGS) -c amc.cpp
main.o: amc.h main.h main.cpp
	$(CXX) -o main.o $(FLAGS) -c main.cpp

blobs: main.o amc.o
	$(CXX) -o blobs main.o amc.o $(FLAGS) $(LIBS)

clean:
	rm -f *.o core $(ALL)
