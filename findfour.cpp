#include "lib/Mouse.h"
#include "lib/Vga.h"
#include "game.h"

int main() {
    
    Mouse* mouse = Mouse::getInstance();
    game game(320, 200);

    if(!game.initialize()) {
        return -1;
    }

    while (1) {
        game.update();

        if (mouse->getRight()) {
            break;
        }
    }

    game.shutdown();

    return 0;
} 