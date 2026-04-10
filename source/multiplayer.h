#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <3ds.h>
#include <citro2d.h>

void initMultiplayer();
void updateMultiplayer(u32 kDown, touchPosition touch);
void drawMultiplayerTop(C3D_RenderTarget* top);
void drawMultiplayerBottom(C3D_RenderTarget* bottom);
void resetMultiplayerBoard();

// Reuse functions from game.cpp
bool isValidMove(int col);
int dropPiece(int col, int player);
bool checkWin(int piece);
bool isBoardFull();

#endif