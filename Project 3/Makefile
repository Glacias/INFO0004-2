CC = g++
CFLAGS = -std=c++14 -Wall -Wextra -O3
CFILES = difference.cpp union.cpp rotation.cpp shift.cpp triangle.cpp ellipse.cpp rectangle.cpp circle.cpp point.cpp image.cc Token.cpp parser.cpp
OUT = painter

painter : parser.cpp
	$(CC) $(CFLAGS) $(CFILES) -o $(OUT)
