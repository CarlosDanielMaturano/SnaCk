#include <stdio.h>

void clear_screen();

int main(void) {
    for (;;)
        clear_screen();
    return 0;
}

void clear_screen() {
    printf("\e[1;1H\e[2J");
}
