#pragma once

#include <stdint.h>

class Vga
{
public:
    static uint8_t *getAddress();
    static void getPaletteEntry(uint8_t index, uint8_t *r, uint8_t *g, uint8_t *b);
    static void setPaletteEntry(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    static void setPaletteMask(uint8_t mask);
    static void setMode(uint8_t mode);
    static void verticalSync();
};
