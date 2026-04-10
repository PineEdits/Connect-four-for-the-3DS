// source/game.h
#pragma once
#include <3ds.h>
#include <citro2d.h>

// Screen dimensions
#define TOP_WIDTH     400
#define TOP_HEIGHT    240
#define BOTTOM_WIDTH  320
#define BOTTOM_HEIGHT 240

// Connect Four board
#define COLS      7
#define ROWS      6
#define CELL_SIZE 32
#define BOARD_X   ((TOP_WIDTH - (COLS * CELL_SIZE)) / 2)
#define BOARD_Y   20

// Colors
#define COLOR_BOARD     C2D_Color32(0, 80, 180, 255)
#define COLOR_RED       C2D_Color32(220, 40, 40, 255)
#define COLOR_YELLOW    C2D_Color32(240, 220, 40, 255)
#define COLOR_WHITE     C2D_Color32(255, 255, 255, 255)
#define COLOR_HIGHLIGHT C2D_Color32(255, 255, 100, 255)

enum GameState { STATE_PLAYING, STATE_WIN_RED, STATE_WIN_YELLOW, STATE_DRAW };

void initGame();
void resetGameBoard();
void updateGame(u32 kDown, touchPosition touch);
void drawGameTop(C3D_RenderTarget* top);
void drawGameBottom(C3D_RenderTarget* bottom);
bool isGameOver();