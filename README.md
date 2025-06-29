# Find Four

## Overview

Find Four is a graphical implementation of the classic Connect Four game, developed for an emulated x86 real mode (DOS) environment. The game demonstrates efficient use of buffered memory techniques, direct video memory manipulation, and mouse input handling to provide smooth gameplay with optimized graphical updates.

This project leverages VGA mode 0x13 (320x200, 256 colors) and operates by loading raw image and palette files, managing a back buffer for rendering, and employing a dirty rectangles approach to update only modified regions of the screen. Mouse input is polled continuously to enable interactive chip placement.

---

## Features

- Direct manipulation of VGA video memory using double buffering  
- Use of dirty rectangles to minimize redraw overhead and improve performance  
- Mouse input handling via polling of the provided `Mouse` driver API  
- Loading and rendering of raw VGA image files and palette data  
- Gameplay logic implementing turn-based chip placement in a 7x6 grid  
- Win detection for vertical, horizontal, and diagonal four-in-a-row  
- Tie detection when the board is full  
- Graceful shutdown and mode restoration

---

## Requirements

- Open Watcom C/C++ compiler suite (or compatible)  
- x86 real mode or DOS environment (emulated or physical) supporting VGA mode 0x13  
- Mouse driver and VGA support as provided by `Mouse` and `Vga` libraries  
- Raw data files in `data/` directory (palette and VGA images)  

---

