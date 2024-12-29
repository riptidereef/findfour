#include "game.h"

game::game(int screenWidth, int screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    backBuffer = NULL;
    blueTile = NULL;
    redTile = NULL;
    boardTile = NULL;
    mousePressed = false;
    mouseReleased = false;
    oldMousePress = false;
    newSelectedCol = -1;
    oldSelectedCol = -1;
    tilesPlaced = 0;
    blueWin = false;
    redWin = false;
    tie = false;
    gameDone = false;
}

game::~game() {
    shutdown();
}

bool game::initialize() {
    // Verify that the mouse is available, and if not, terminate
    mouse = Mouse::getInstance();
    if (!mouse) {
        return false;
    }
    mouse->reset(screenWidth, screenHeight, 4);

    // Load the palette and image files, terminating on error
    ifstream paletteFile("data/findfour.pal", ios::binary);
    if (!paletteFile) {
        return false;
    }
    ifstream blueFile("data/blue.vga", ios::binary);
    if (!blueFile) {
        return false;
    }
    ifstream redFile("data/red.vga", ios::binary);
    if (!redFile) {
        return false;
    }
    ifstream boardFile("data/board.vga", ios::binary);
    if (!boardFile) {
        return false;
    }

    redTile = new uint8_t[TILE_SIZE];
    blueTile = new uint8_t[TILE_SIZE];
    boardTile = new uint8_t[TILE_SIZE];

    uint8_t blueWidth, blueHeight, redWidth, redHeight, boardWidth, boardHeight;
    blueFile.read(reinterpret_cast<char*>(&blueWidth), 1);
    blueFile.read(reinterpret_cast<char*>(&blueHeight), 1);
    blueFile.read(reinterpret_cast<char*>(blueTile), TILE_SIZE);

    redFile.read(reinterpret_cast<char*>(&redWidth), 1);
    redFile.read(reinterpret_cast<char*>(&redHeight), 1);
    redFile.read(reinterpret_cast<char*>(redTile), TILE_SIZE);

    boardFile.read(reinterpret_cast<char*>(&boardWidth), 1);
    boardFile.read(reinterpret_cast<char*>(&boardHeight), 1);
    boardFile.read(reinterpret_cast<char*>(boardTile), TILE_SIZE);

    // Change to graphical mode
    Vga::setMode(GRAPHICAL_MODE);

    // Load the palette into the VGA controller
    uint8_t palette[768];
    paletteFile.read(reinterpret_cast<char*>(palette), 768);
    if (!paletteFile) {
        return false;
    }
    for (int i = 0; i < 256; i++) {
        Vga::setPaletteEntry(i, palette[i * 3], palette[i * 3 + 1], palette[i * 3 + 2]);
    }

    // Clear the screen and back buffer using Color 0, then draw the game board on the back buffer
    videoMemory = Vga::getAddress();
    if (!videoMemory) {
        return false;
    }

    backBuffer = new uint8_t[WINDOW_SIZE];
    memcpy(backBuffer, defaultScreen, defaultScreen_len);
    memcpy(videoMemory, defaultScreen, defaultScreen_len);

    // Initialize the mouse position to the center of the screen
    mouseX = 160;
    mouseY = 100;

    // Set the mouse cursor to be the chip of the blue player (the starting player)
    drawCursor();

    // Draw the cursor on the back buffer, centered on the mouse position
    // DONE

    // Set the current player to blue and start the update loop
    currPlayer = BLUE;

    return true;
}

void game::update() {
    // Update the mouse state
    mouse->update();

    // Get the current mouse coordinates
    oldMouseX = mouseX;
    oldMouseY = mouseY;
    mouseX = mouse->getX();
    mouseY = mouse->getY();

    // Get mouse press down
    if (!oldMousePress && mouse->getLeft()) {
        oldSelectedCol = (mouse->getX() - 48) / 32;
        mousePressed = true;
    }
    else {
        mousePressed = false;
    }

    // Get mouse release
    if (oldMousePress && !mouse->getLeft()) {
        newSelectedCol = (mouse->getX() - 48) / 32;
        mouseReleased = true;
    }
    else {
        mouseReleased = false;
    }

    oldMousePress = mouse->getLeft();

    clearCursor();
    clearDirtyTiles();

    if (mouseReleased && (oldSelectedCol == newSelectedCol) && !gameDone && !tie) {
        handleMousePress(oldMouseX, oldMouseY);
    }
    else if (gameDone || tie) {
        drawGameDone();
    }

    drawDirtyTiles();
    drawCursor();

    Vga::verticalSync();
    updateVideo();
}

void game::shutdown() {
    Mouse::shutdown();
    Vga::setMode(TEXT_MODE);

    delete[] backBuffer;
    delete[] blueTile;
    delete[] redTile;
    delete[] boardTile;

    backBuffer = NULL;
    blueTile = NULL;
    redTile = NULL;
    boardTile = NULL;
}

void game::updateVideo() {
    memcpy(videoMemory, backBuffer, WINDOW_SIZE);
}

void game::blit(uint8_t* source, int srcWidth, int srcHeight, uint8_t* dest, int destWidth, int destHeight, int destX, int destY) {

    for (int y = 0; y < srcHeight; y++) {
        for (int x = 0; x < srcWidth; x++) {
            int destPixelX = destX + x;
            int destPixelY = destY + y;

            if (destPixelX >= 0 && destPixelX < destWidth && 
                destPixelY >= 0 && destPixelY < destHeight) {
                uint8_t color = source[y * srcWidth + x];
                if (color != 0) {  // Skip transparent color
                    dest[destPixelY * destWidth + destPixelX] = color;
                }
            }
        }
    }
}

void game::clearRect(int x, int y, int width, int height) {
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    
    if (x + width > screenWidth) {
        width = screenWidth - x; 
    }

    if (y + height > screenHeight) {
        height = screenHeight - y;
    }

    for (int i = 0; i < height; i++) {
        memset(&backBuffer[(y + i) * screenWidth + x], 0, width);
    }
}

void game::clearCursor() {
    clearRect(oldMouseX - 16, oldMouseY - 16, 32, 32);
}

void game::drawCursor() {
    int tileX = mouseX - 16;
    int tileY = mouseY - 16;

    if (currPlayer == BLUE) {
        blit(blueTile, 32, 32, backBuffer, screenWidth, screenHeight, tileX, tileY);
    }
    else if (currPlayer == RED) {
        blit(redTile, 32, 32, backBuffer, screenWidth, screenHeight, tileX, tileY);
    }
}

void game::clearDirtyTiles() {
    int gridX = (oldMouseX - 48) / 32;
    int gridY = (oldMouseY - 4) / 32;

    int screenGridX = 48 + 32 * gridX;
    int screenGridY = 4 + 32 * gridY;

    int xGridOffset = oldMouseX - screenGridX;
    int yGridOffset = oldMouseY - screenGridY;

    if (xGridOffset < 16 && yGridOffset < 16) {
        clearGridTile(gridX, gridY);
        clearGridTile(gridX - 1, gridY);
        clearGridTile(gridX, gridY - 1);
        clearGridTile(gridX - 1, gridY - 1);
    }
    else if (xGridOffset < 16 && yGridOffset >= 16) {
        clearGridTile(gridX, gridY);
        clearGridTile(gridX, gridY + 1);
        clearGridTile(gridX - 1, gridY);
        clearGridTile(gridX - 1, gridY + 1);
    }
    else if (xGridOffset >= 16 && yGridOffset < 16) {
        clearGridTile(gridX, gridY);
        clearGridTile(gridX + 1, gridY);
        clearGridTile(gridX, gridY - 1);
        clearGridTile(gridX + 1, gridY - 1);
    }
    else if (xGridOffset >= 16 && yGridOffset >= 16) {
        clearGridTile(gridX, gridY);
        clearGridTile(gridX + 1, gridY);
        clearGridTile(gridX, gridY + 1);
        clearGridTile(gridX + 1, gridY + 1);
    }
}

void game::drawDirtyTiles() {
    int gridX = (oldMouseX - 48) / 32;
    int gridY = (oldMouseY - 4) / 32;

    int screenGridX = 48 + 32 * gridX;
    int screenGridY = 4 + 32 * gridY;

    int xGridOffset = oldMouseX - screenGridX;
    int yGridOffset = oldMouseY - screenGridY;

    if (xGridOffset < 16 && yGridOffset < 16) {
        drawGridTile(gridX, gridY);
        drawGridTile(gridX - 1, gridY);
        drawGridTile(gridX, gridY - 1);
        drawGridTile(gridX - 1, gridY - 1);
    }
    else if (xGridOffset < 16 && yGridOffset >= 16) {
        drawGridTile(gridX, gridY);
        drawGridTile(gridX, gridY + 1);
        drawGridTile(gridX - 1, gridY);
        drawGridTile(gridX - 1, gridY + 1);
    }
    else if (xGridOffset >= 16 && yGridOffset < 16) {
        drawGridTile(gridX, gridY);
        drawGridTile(gridX + 1, gridY);
        drawGridTile(gridX, gridY - 1);
        drawGridTile(gridX + 1, gridY - 1);
    }
    else if (xGridOffset >= 16 && yGridOffset >= 16) {
        drawGridTile(gridX, gridY);
        drawGridTile(gridX + 1, gridY);
        drawGridTile(gridX, gridY + 1);
        drawGridTile(gridX + 1, gridY + 1);
    }
}

void game::clearGridTile(int gridX, int gridY) {
    if (gridX >= 0 && gridX < 7 && gridY >= 0 && gridY < 6) {
        int screenX = 48 + 32 * gridX;
        int screenY = 4 + 32 * gridY;
        clearRect(screenX, screenY, 32, 32); 
    }
}

void game::drawGridTile(int gridX, int gridY) {
    if (gridX >= 0 && gridX < 7 && gridY >= 0 && gridY < 6) {
        int screenX = 48 + 32 * gridX;
        int screenY = 4 + 32 * gridY;

        player gridPlayer = board[gridY][gridX];

        if (gridPlayer == BLUE) {
            blit(blueTile, 32, 32, backBuffer, screenWidth, screenHeight, screenX, screenY);   
        }
        else if (gridPlayer == RED) {
            blit(redTile, 32, 32, backBuffer, screenWidth, screenHeight, screenX, screenY);
        }

        blit(boardTile, 32, 32, backBuffer, screenWidth, screenHeight, screenX, screenY);
    }
}

void game::handleMousePress(int screenX, int screenY) {
    int gridX = (screenX - 48) / 32;
    int gridY = (screenY - 4) / 32;

    if (insertChip(gridX)) {
        // Switch player
        bool winner = checkWin();

        if (winner) {
            if (currPlayer == RED) {
                redWin = true;
            }
            else if (currPlayer == BLUE) {
                blueWin = true;
            }
            gameDone = true;
            return;
        }

        tie = checkTie();

        if (tie) {
            gameDone = true;
            return;
        }

        if (currPlayer == RED) {
            currPlayer = BLUE;
        }
        else {
            currPlayer = RED;
        }
    }
}

bool game::insertChip(int col) {
    if (col < 0 || col > 7) {
        return false;
    }
    else if (board[0][col] != EMPTY) {
        return false;
    }
    else {
        int currRow = 0;
        while (currRow != 5 && board[currRow + 1][col] == EMPTY) {
            currRow++;
        }

        board[currRow][col] = currPlayer;
        tilesPlaced++;

        drawGridTile(col, currRow);
        return true;
    }
}

bool game::checkWin() {
    const int rows = 6;
    const int cols = 7;

    // Check horizontal wins
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x <= cols - 4; x++) {
            if (board[y][x] == currPlayer &&
                board[y][x + 1] == currPlayer &&
                board[y][x + 2] == currPlayer &&
                board[y][x + 3] == currPlayer) {
                return true;
            }
        }
    }

    // Check vertical wins
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y <= rows - 4; y++) {
            if (board[y][x] == currPlayer &&
                board[y + 1][x] == currPlayer &&
                board[y + 2][x] == currPlayer &&
                board[y + 3][x] == currPlayer) {
                return true;
            }
        }
    }

    // Check diagonal (bottom-left to top-right)
    for (int x = 0; x <= cols - 4; x++) {
        for (int y = 0; y <= rows - 4; y++) {
            if (board[y][x] == currPlayer &&
                board[y + 1][x + 1] == currPlayer &&
                board[y + 2][x + 2] == currPlayer &&
                board[y + 3][x + 3] == currPlayer) {
                return true;
            }
        }
    }

    // Check diagonal (top-left to bottom-right)
    for (int x = 0; x <= cols - 4; x++) {
        for (int y = 3; y < rows; y++) {
            if (board[y][x] == currPlayer &&
                board[y - 1][x + 1] == currPlayer &&
                board[y - 2][x + 2] == currPlayer &&
                board[y - 3][x + 3] == currPlayer) {
                return true;
            }
        }
    }

    return false;
}

bool game::checkTie() {
    return tilesPlaced >= 42;
}

void game::drawGameDone() {

    uint8_t* leftTile;
    uint8_t* rightTile;

    if (redWin) {
        leftTile = redTile;
        rightTile = redTile;
    }
    else if (blueWin) {
        leftTile = blueTile;
        rightTile = blueTile;
    }
    else if (tie) {
        leftTile = blueTile;
        rightTile = redTile;
    }

    blit(leftTile, 32, 32, backBuffer, screenWidth, screenHeight, 8, 84);
    blit(rightTile, 32, 32, backBuffer, screenWidth, screenHeight, 280, 84);
}









void game::saveScreenToFile(const std::string& filename) {
    long num1 = 32000;
    long num2 = 32000;

    ofstream fs("screen.bin", ios::out | ios::binary | ios::app);
    fs.write(reinterpret_cast<const char*>(backBuffer), num1);
    fs.write(reinterpret_cast<const char*>(backBuffer+32000), num2);
    fs.close();
}

void game::printBoard() {
    console << "Current Board State:" << endl;
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            char symbol;
            switch (board[row][col]) {
                case RED:   symbol = 'R'; break;
                case BLUE:  symbol = 'B'; break;
                case EMPTY: symbol = 'X'; break;
            }
            console << symbol << " ";
        }
        console << endl;
    }
    console << endl;
}

