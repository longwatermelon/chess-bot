all:
	g++ -std=c++17 src/*.cpp -ggdb -lSDL2 -lSDL2_image -Isrc/ -O3
