gol:
	cc -o gol main.c `sdl2-config --cflags --libs`
clean:
	rm gol
