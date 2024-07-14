#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <getopt.h>
#include <ctype.h>

int bx = 50, by = 50;
int delay_time = 100;
int survival[] = {2, 3, 0, 0, 0, 0, 0, 0, 0, 0};
int s_size = 2;
int birth[] = {3, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int b_size = 1;
int invert = 1;
int cell_size = 10;
unsigned char *board;
unsigned char *counts;
SDL_Window *win;
SDL_Renderer *ren;

void delay(int milli_seconds) {
    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds * 1000) { ;
    }
}

void draw(unsigned char board[bx * by]) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_Rect rect;
    rect.w = cell_size;
    rect.h = cell_size;
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    for (int x = 0; x < bx; x++) {
        for (int y = 0; y < by; y++) {
            if (board[y * bx + x] == invert) {
                rect.x = x*cell_size;
                rect.y = y*cell_size;
                SDL_RenderFillRect(ren, &rect);
            }
        }
    }
    SDL_RenderPresent(ren);
}

int count_live_neighbours(unsigned char board[bx * by], int x, int y) {
    return \
           board[((y+by - 1) % by) * bx + ((x+bx - 1) % bx)] \
         + board[((y+by - 1) % by) * bx +   x              ] \
         + board[((y+by - 1) % by) * bx + ((x+bx + 1) % bx)] \
         + board[  y               * bx + ((x+bx - 1) % bx)] \
         + board[  y               * bx + ((x+bx + 1) % bx)] \
         + board[((y+by + 1) % by) * bx + ((x+bx - 1) % bx)] \
         + board[((y+by + 1) % by) * bx +   x              ] \
         + board[((y+by + 1) % by) * bx + ((x+bx + 1) % bx)];
}

int isNumber(char *s) {
    int is = 1;
    int z = 0;
    while (z<strlen(s) && is == 1) {
        if (s[z] < "0"[0] || s[z] > "9"[0]) {
            is = 0;
        }
        z++;
    }
    return is;
}

int isinarray(int val, int *arr, size_t size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == val) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "x:y:d:s:b:r:i")) != -1) {
        switch (c) {
            case 'x':
                if (isNumber(optarg)) {
                    if (atoi(optarg) != 0) {
                        bx = atoi(optarg);
                    } else {
                        fprintf (stderr, "Option -%c cannot be zero.\n", optopt);
                        return 1;
                    }
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 'y':
                if (isNumber(optarg)) {
                    if (atoi(optarg) != 0) {
                        by = atoi(optarg);
                    } else {
                        fprintf (stderr, "Option -%c cannot be zero.\n", optopt);
                        return 1;
                    }
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 'd':
                if (isNumber(optarg)) {
                    delay_time = atoi(optarg);
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 's':
                if (isNumber(optarg)) {
                    s_size = strlen(optarg);
                    for (int z = 0; z < s_size; z++) {
                        survival[z] = optarg[z] - '0';
                    }
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 'b':
                if (isNumber(optarg)) {
                    b_size = strlen(optarg);
                    for (int z = 0; z < b_size; z++) {
                        birth[z] = optarg[z] - '0';
                    }
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 'i':
                invert = 0;
                break;
            case 'r':
                if (isNumber(optarg)) {
                    cell_size = atoi(optarg);
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case '?':
                if (optopt == 'x' || optopt == 'y' || optopt == 'd') {
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint (optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort ();
        }
    }
    srand(time(0));
    board = (unsigned char *)malloc(sizeof(unsigned char) * bx * by);
    for (int x = 0; x < bx; x++) {
        for (int y = 0; y < by; y++) {
            board[y * bx + x] = rand() % 3 % 2;
        }
    }
    counts = (unsigned char *)malloc(sizeof(unsigned char) * bx * by);
    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, bx*cell_size, by*cell_size, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    draw(board);
    delay(delay_time);
    int running = 1;
    int once = 0;
    int quit = 0;
    SDL_Event e;
    while (!quit) {
        if (running) {
            // count neighbors
            for (int x = 0; x < bx; x++) {
                for (int y = 0; y < by; y++) {
                    counts[y * bx + x] = count_live_neighbours(board, x, y);
                }
            }
            // update the board from counts
            for (int x = 0; x < bx; x++) {
                for (int y = 0; y < by; y++) {
                    if (board[y * bx + x]) {
                        if (!isinarray(counts[y * bx + x], survival, s_size)) {
                            board[y * bx + x] = 0;
                        }
                    } else {
                        if (isinarray(counts[y * bx + x], birth, b_size)) {
                            board[y * bx + x] = 1;
                        }
                    }
                }
            }
            draw(board);
            if (once) {
                running = 0;
                once = 0;
            } else {
                delay(delay_time);
            }
        }
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case ' ':
                            running = !running;
                            break;
                        case 'q':
                            quit = 1;
                            break;
                        case 'r':
                            for (int x = 0; x < bx; x++) {
                                for (int y = 0; y < by; y++) {
                                    board[y * bx + x] = rand() % 3 % 2;
                                }
                            }
                            draw(board);
                            break;
                        case 'c':
                            for (int x = 0; x < bx; x++) {
                                for (int y = 0; y < by; y++) {
                                    board[y * bx + x] = 0;
                                }
                            }
                            draw(board);
                            break;
                        case 'i':
                            for (int x = 0; x < bx; x++) {
                                for (int y = 0; y < by; y++) {
                                    board[y * bx + x] = !board[y * bx + x];
                                }
                            }
                            draw(board);
                            break;
                        case 's':
                            running = 1;
                            once = 1;
                            break;
                    }
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == 1) {
                        if (e.button.x/cell_size < bx && e.button.y/cell_size < by) {
                            board[e.button.y/cell_size * bx + e.button.x/cell_size] = !board[e.button.y/cell_size * bx + e.button.x/cell_size];
                            draw(board);
                        }
                    }
                    break;
            }
        }
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
