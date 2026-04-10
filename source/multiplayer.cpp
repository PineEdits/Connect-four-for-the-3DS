#include "multiplayer.h"
#include "game.h"

C2D_TextBuf mgameTextBuf;
C2D_Text mtextRedTurn, mtextYellowTurn, mtextWinRed, mtextWinYellow, mtextDraw, mtextControls;

extern int board[ROWS][COLS];
extern int currentPlayer;
extern GameState gameState;
extern int selectedCol;

bool wasTouchingMulti = false;

void initMultiplayer() {
    mgameTextBuf = C2D_TextBufNew(4096);
    C2D_TextParse(&mtextRedTurn,  mgameTextBuf, "RED TURN");
    C2D_TextParse(&mtextYellowTurn,     mgameTextBuf, "YELLOW TURN");
    C2D_TextParse(&mtextWinRed,     mgameTextBuf, "RED WINS!");
    C2D_TextParse(&mtextWinYellow,  mgameTextBuf, "YELLOW WINS!");
    C2D_TextParse(&mtextDraw,       mgameTextBuf, "DRAW!");
    C2D_TextParse(&mtextControls,   mgameTextBuf, "Controls:\n"
                                            "Left/Right or Joystick: select column\n"
                                            "A or tap bottom screen: drop piece\n"
                                            "SELECT : restart\n"
                                            "START  : exit");
}

void resetMultiplayerBoard() {
    memset(board, 0, sizeof(board));
    currentPlayer = 1;
    selectedCol = 3;
    gameState = STATE_PLAYING;
    wasTouchingMulti = false;
}

void updateMultiplayer(u32 kDown, touchPosition touch) {
    if (gameState != STATE_PLAYING) return;

    if (kDown & (KEY_LEFT | KEY_DLEFT))  selectedCol = (selectedCol + COLS - 1) % COLS;
    if (kDown & (KEY_RIGHT | KEY_DRIGHT)) selectedCol = (selectedCol + 1) % COLS;

    bool isTouching = (touch.px != 0 || touch.py != 0);

    if (isTouching) {
        selectedCol = (touch.px * COLS) / BOTTOM_WIDTH;
        if (selectedCol < 0) selectedCol = 0;
        if (selectedCol >= COLS) selectedCol = COLS-1;
    } 
    else if (wasTouchingMulti) {
        int row = dropPiece(selectedCol, currentPlayer);
        if (row != -1) {
            if (checkWin(currentPlayer)) {
                gameState = (currentPlayer == 1) ? STATE_WIN_RED : STATE_WIN_YELLOW;
            } else if (isBoardFull()) {
                gameState = STATE_DRAW;
            } else {
                currentPlayer = (currentPlayer == 1) ? 2 : 1;
            }
        }
    }

    wasTouchingMulti = isTouching;

    if (kDown & KEY_A) {
        int row = dropPiece(selectedCol, currentPlayer);
        if (row != -1) {
            if (checkWin(currentPlayer)) {
                gameState = (currentPlayer == 1) ? STATE_WIN_RED : STATE_WIN_YELLOW;
            } else if (isBoardFull()) {
                gameState = STATE_DRAW;
            } else {
                currentPlayer = (currentPlayer == 1) ? 2 : 1;
            }
        }
    }
}

void drawMultiplayerTop(C3D_RenderTarget* top) {
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(20, 20, 60, 255));

    C2D_DrawRectSolid(BOARD_X-6, BOARD_Y-6, 0.0f, COLS*CELL_SIZE+12, ROWS*CELL_SIZE+12, C2D_Color32(10,10,30,255));

    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLS; c++) {
            float x = BOARD_X + c * CELL_SIZE;
            float y = BOARD_Y + r * CELL_SIZE;
            C2D_DrawRectSolid(x, y, 0.0f, CELL_SIZE, CELL_SIZE, COLOR_BOARD);
            C2D_DrawCircleSolid(x + CELL_SIZE/2, y + CELL_SIZE/2, 0.0f, CELL_SIZE/2 - 4, COLOR_WHITE);

            if (board[r][c] == 1) C2D_DrawCircleSolid(x + CELL_SIZE/2, y + CELL_SIZE/2, 0.0f, CELL_SIZE/2 - 4, COLOR_RED);
            if (board[r][c] == 2) C2D_DrawCircleSolid(x + CELL_SIZE/2, y + CELL_SIZE/2, 0.0f, CELL_SIZE/2 - 4, COLOR_YELLOW);
        }
    }

    if (gameState == STATE_PLAYING) {
        float ax = BOARD_X + selectedCol * CELL_SIZE + CELL_SIZE/2.0f;
        float ay = BOARD_Y - 22.0f;
        C2D_DrawTriangle(ax-12, ay+12, COLOR_HIGHLIGHT,
                         ax+12, ay+12, COLOR_HIGHLIGHT,
                         ax,    ay,     COLOR_HIGHLIGHT, 0.0f);
    }

    if (gameState == STATE_WIN_RED) {
        C2D_DrawRectSolid(0, 90, 0.0f, 400, 60, C2D_Color32(220, 20, 20, 100));   // red strip
        C2D_DrawText(&mtextWinRed, C2D_AlignCenter, 200, 100, 0.0f, 2.0f, 2.0f);
    } else if (gameState == STATE_WIN_YELLOW) {
        C2D_DrawRectSolid(0, 90, 0.0f, 400, 60, C2D_Color32(255, 215, 0, 100));   // yellow strip
        C2D_DrawText(&mtextWinYellow, C2D_AlignCenter, 200, 100, 0.0f, 2.0f, 2.0f);
    } else if (gameState == STATE_DRAW) {
        C2D_DrawRectSolid(0, 90, 0.0f, 400, 60, C2D_Color32(100, 100, 100, 100));   // gray strip
        C2D_DrawText(&mtextDraw, C2D_AlignCenter, 200, 100, 0.0f, 2.0f, 2.0f);
    }
}

void drawMultiplayerBottom(C3D_RenderTarget* bottom) {
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(124, 118, 106, 255));

    if (currentPlayer == 1 && gameState == STATE_PLAYING) {
        // Red strip for Player's turn
        C2D_DrawRectSolid(0, 0, 0.0f, 400, 45, C2D_Color32(220, 20, 20, 255));
    } else if (currentPlayer == 2 && gameState == STATE_PLAYING) {
        // Yellow strip for AI's turn
        C2D_DrawRectSolid(0, 0, 0.0f, 400, 45, C2D_Color32(255, 215, 0, 255));
    } else {
        // Gray strip for game over
        C2D_DrawRectSolid(0, 0, 0.0f, 400, 45, C2D_Color32(255, 255, 255, 255));
    }

    if (gameState == STATE_PLAYING) {
        C2D_Text* t = (currentPlayer == 1) ? &mtextRedTurn : &mtextYellowTurn;   // reuse from game
        C2D_DrawText(t, C2D_AlignCenter, 160, 20, 0.0f, 0.8f, 0.8f);
    }

    C2D_DrawText(&mtextControls, 0, 15, 65, 0.0f, 0.65f, 0.65f);
}