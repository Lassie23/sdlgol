gol:
	cc -o gol main.c `sdl2-config --cflags --libs` -march=native -Ofast
clean:
	rm gol
