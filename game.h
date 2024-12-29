#pragma once
#include "lib/Vga.h"
#include "lib/Mouse.h"
#include "data/initialScreen.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>
using namespace std;

#define GRAPHICAL_MODE 0x13
#define TEXT_MODE      0x03
#define WINDOW_SIZE    64000
#define TILE_SIZE      1024

enum player {RED, BLUE, EMPTY};

player board[6][7] = {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};

class game {
    private:
        int screenWidth;
        int screenHeight;
        Mouse* mouse;
        uint8_t* videoMemory;
        int mouseX;
        int oldMouseX;
        int mouseY;
        int oldMouseY;
        player currPlayer;
        uint8_t* blueTile;
        uint8_t* redTile;
        uint8_t* boardTile;
        uint8_t* backBuffer;

        ofstream console;

        bool oldMousePress;
        bool mousePressed;
        bool mouseReleased;

        int newSelectedCol;
        int oldSelectedCol;
        
        int tilesPlaced;
        bool blueWin;
        bool redWin;
        bool tie;
        bool gameDone;

        bool loadTile(const string& filePath, uint8_t*& tileBuffer);
        void updateVideo();
        void blit(uint8_t* source, int srcWidth, int srcHeight, 
                  uint8_t* dest, int destWidth, int destHeight, 
                  int destX, int destY);
        void clearRect(int x, int y, int width, int height);
        void clearCursor();
        void drawCursor();
        void clearDirtyTiles();
        void drawDirtyTiles();
        void clearGridTile(int gridX, int gridY);
        void drawGridTile(int gridX, int gridY);
        void handleMousePress(int screenX, int screenY);
        bool insertChip(int col);
        bool checkWin();
        bool checkTie();
        void drawGameDone();
        
    public:
        void saveScreenToFile(const std::string& filename);
        void printBoard();
        game(int screenWidth, int screenHeight);
        ~game();
        bool initialize();
        void update();
        void shutdown();
};