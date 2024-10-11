#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <termios.h>
#include <fcntl.h>

#define EMPTY 0
#define SNAKE_BODY 1
#define SNAKE_HEAD 2
#define FOOD 3 
#define MAP_WIDTH 30
#define MAP_HEIGHT 15
#define INITIAL_SNAKE_BODY_SIZE 5
#define SLEEP_TIME 140000 // 16 miliseconds in microseconds

typedef struct {
    int x;
    int y;
} Vector2;

typedef struct {
    Vector2 head_dir;
    Vector2 *head_pos;
    int body_size;
    Vector2 *body;
} Snake;

void clear_screen();
void draw_map(int [MAP_HEIGHT][MAP_WIDTH]);
void clear_map();
void move_snake(Snake *);
void handle_input(Snake *);
void draw_snake(Snake *);
void grow_snake(Snake *);
void spawn_food(Snake *);
void apply_food_collision(Snake *);
Snake create_snake();
int _kbhit();
int check_snake_collision(Snake *);

int map[MAP_HEIGHT][MAP_WIDTH];
Vector2 *snake_body;
Vector2 food_pos;

int main(void) {
    Snake snake = create_snake();
    struct timeval start, current;
    double elapsed_time_ms = 0;
    gettimeofday(&start, NULL);

    for (;;) {
        gettimeofday(&current, NULL);
        elapsed_time_ms =  (current.tv_sec - start.tv_sec) * 1000 
            + (current.tv_usec - start.tv_usec) / 1000;
    
        if (elapsed_time_ms >= 300) 
            start = current;

        clear_screen();
        handle_input(&snake);
        move_snake(&snake);

        if (check_snake_collision(&snake))
            return 1;

        apply_food_collision(&snake);
        draw_snake(&snake);
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
    map[food_pos.y][food_pos.x] = FOOD;

    for (size_t j = 0; j < MAP_HEIGHT; j++)  {
        for (size_t i = 0; i < MAP_WIDTH; i++) 
            switch(map[j][i]) {
            case 0:
                putchar('+');
                break;
            case 1:
                putchar('$');
                break;
            case 2:
                putchar('@');
                break;
            case 3:
                putchar('S');
                break;
            }
        putchar('\n');
    }
}

void clear_map() {
    for (size_t j = 0; j < MAP_HEIGHT; j++)  
        for (size_t i = 0; i < MAP_WIDTH; i++) 
            map[j][i] = 0;
};

Snake create_snake() {
    snake_body = malloc(INITIAL_SNAKE_BODY_SIZE * sizeof(Vector2));
    for (size_t i = 0; i < INITIAL_SNAKE_BODY_SIZE; i++) {
        snake_body[i] = (Vector2) { i+1, 5 };
    }
    Vector2 snake_head_dir = (Vector2) { 1, 0 };
    Vector2 *snake_head_pos = &snake_body[INITIAL_SNAKE_BODY_SIZE - 1];
    return (Snake) {
        .head_dir = snake_head_dir,
        .head_pos = snake_head_pos,
        .body_size = INITIAL_SNAKE_BODY_SIZE,
        .body = snake_body,
    };
}

void move_snake(Snake *snake) {
    Vector2 new_head = (Vector2) {
        .x = snake->head_pos->x + snake->head_dir.x,
        .y = snake->head_pos->y + snake->head_dir.y,
    };

    // wrap the new_head pos
    if (new_head.y >= MAP_HEIGHT) new_head.y = 0;
    if (new_head.x >= MAP_WIDTH) new_head.x = 0;
    if (new_head.x < 0) new_head.x = MAP_WIDTH - 1;
    if (new_head.y < 0) new_head.y = MAP_HEIGHT - 1;

    size_t body_size = snake->body_size;
    for (size_t i = 0; i < body_size; i++)
        snake->body[i-1] = snake->body[i];
    snake->body[body_size - 1] = new_head;
}

void handle_input(Snake *snake) {
    Vector2 *dir = &(snake->head_dir);
    if (_kbhit()) {
        char c = getchar() | (1 << 5);
        switch (c) {
        case 'e':
            grow_snake(snake);
            break;
        case 'w':
            if (dir->y == 0) {
                dir->x = 0; 
                dir->y = -1;
            }
            break;
        case 'a':
            if (dir->x == 0) {
                dir->y = 0; 
                dir->x = -1;
            }
            break;
        case 's':
            if (dir->y == 0) {
                dir->x = 0; 
                dir->y = 1;
            }
            break;
        case 'd':
            if (dir->x == 0) {
                dir->y = 0; 
                dir->x = 1;
            }
            break;
        case 'r':
            spawn_food(snake);
            break;
        default:
            return;
        }
    }
}

int _kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF) {
      ungetc(ch, stdin);
      return 1;
    }

    return 0;
}

void draw_snake(Snake *snake) {
    for (size_t i = 0; i < snake->body_size; i++) {
        Vector2 *pos = &snake_body[i];
        map[pos->y][pos->x] = (pos == snake->head_pos ? 2 : 1);
    }
}

void grow_snake(Snake *snake) {
    size_t old_body_size = snake->body_size;
    size_t new_body_size = old_body_size + 1;
    
    // allocate more memory 
    Vector2 *new_body = malloc(new_body_size * sizeof(Vector2));

    // "clone" the first piece of the snake that 
    // is consumed while the snake is moving
    Vector2 snake_tail = snake_body[0];

    // clone the body leaving a free space and the beginning 
    // of the array
    for (size_t i = 0; i < old_body_size; i++) 
        new_body[i+1] = snake_body[i];

    new_body[0] = snake_tail;

    snake_body = new_body;
    snake->body = snake_body;
    snake->body_size++;
    snake->head_pos = &new_body[new_body_size-1];
}

void spawn_food(Snake *snake) {
    food_pos = (Vector2) { 
        .x = rand() % MAP_WIDTH,
        .y = rand() % MAP_HEIGHT,
    };

    // for avoiding the food spawning inside the snake body
    for (size_t i = 0; i < snake->body_size; i++) {
        if (food_pos.x == snake_body[i].x  && food_pos.y == snake_body[i].y) {
            return spawn_food(snake);
        }
    }
}

void apply_food_collision(Snake *snake) {
    Vector2 head_pos = *snake->head_pos;
    if (head_pos.x == food_pos.x && head_pos.y == food_pos.y) {
        grow_snake(snake);
        spawn_food(snake);
    }
}

int check_snake_collision(Snake *snake) {
    size_t body_size = snake->body_size - 1;
    Vector2 head = snake_body[body_size];
    for (size_t i = 0; i < body_size; i++) {
        if (snake_body[i].x  == head.x && snake_body[i].y == head.y) 
            return 1;
    }
    return 0;
}
