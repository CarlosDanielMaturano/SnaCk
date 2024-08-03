#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define MAP_WIDTH 30
#define MAP_HEIGHT 15
#define INITIAL_SNAKE_BODY_SIZE 3
#define SLEEP_TIME 160000 // 16 miliseconds in microseconds

typedef struct {
    char x;
    char y;
} Vector2;

typedef struct {
    Vector2 head_dir;
    Vector2 head_pos;
    int body_size;
    char *body;
} Snake;

void clear_screen();
void draw_map(int [MAP_HEIGHT][MAP_WIDTH]);
void clear_map();
Snake create_snake();

int map[MAP_HEIGHT][MAP_WIDTH];
char *snake_body;

int main(void) {
    Snake snake = create_snake();
    for (;;) {
        map[snake.head_pos.y][snake.head_pos.x] = 1;
        clear_screen();
        draw_map(map);
        clear_map();
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
            putchar(!map[j][i] ? '+' : '#');
        putchar('\n');
    }
}

void clear_map() {
    for (size_t j = 0; j < MAP_HEIGHT; j++)  
        for (size_t i = 0; i < MAP_WIDTH; i++) 
            map[j][i] = 0;
};

Snake create_snake() {
    snake_body = malloc(1 * sizeof(char));
    Vector2 snake_head_dir = (Vector2) { 1, 0 };
    Vector2 snake_head_pos = (Vector2) { 10, 5 };
    return (Snake) {
        .head_dir = snake_head_dir,
        .head_pos = snake_head_pos,
        .body_size = 1,
        .body = snake_body,
    };
}
