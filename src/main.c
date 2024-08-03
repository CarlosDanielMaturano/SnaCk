#include <stdio.h>
#include <unistd.h>
#define MAP_WIDTH 30
#define MAP_HEIGHT 15
#define SLEEP_TIME 160000 // 16 miliseconds in microseconds

void clear_screen();
void draw_map(int [MAP_HEIGHT][MAP_WIDTH]);

int map[MAP_HEIGHT][MAP_WIDTH];

int main(void) {
    for (;;) {
        clear_screen();
        draw_map(map);
        usleep(SLEEP_TIME);
    }
    return 0;
}

void clear_screen() {
    printf("\e[1;1H\e[2J");
}

void draw_map(int map[MAP_HEIGHT][MAP_WIDTH]) {
    for (size_t j = 0; j < MAP_HEIGHT; j++)  {
        for (size_t i = 0; i < MAP_WIDTH; i++) 
            putchar('+');
        putchar('\n');
    }
}
