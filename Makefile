CCFLAGS = -lstdc++ -std=c++14 -Wall -Wextra -Wpedantic -Werror

all: bin/gba-lz77-compress bin/gba-lz77-decompress

bin/gba-lz77-decompress: io.h io.cpp decompress.cpp Makefile
	clang $(CCFLAGS) decompress.cpp io.cpp -o bin/gba-lz77-decompress

bin/gba-lz77-compress: io.h io.cpp compress.cpp Makefile
	clang $(CCFLAGS) compress.cpp io.cpp -o bin/gba-lz77-compress
