#include "game.h"
#include "menu.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

C2D_TextBuf gameTextBuf;
C2D_Text textHumanTurn, textAITurn, textWinRed, textWinYellow, textDraw, textgControls;

int board[ROWS][COLS] = {0};
int currentPlayer = 1;
GameState gameState = STATE_PLAYING;
int selectedCol = 3;
bool isAIThinking = false;

bool isValidMove(int col) {
    return (col >= 0 && col < COLS && board[0][col] == 0);
}

int dropPiece(int col, int player) {
    if (!isValidMove(col)) return -1;
    for (int r = ROWS-1; r >= 0; r--) {
        if (board[r][col] == 0) {
            board[r][col] = player;
            return r;
        }
    }
    return -1;
}

void undoMove(int col, int row) {
    if (row >= 0) board[row][col] = 0;
}

bool checkWin(int piece) {
    // Horizontal
    for (int c = 0; c < COLS-3; c++) {
        for (int r = 0; r < ROWS; r++) {
            if (board[r][c] == piece && board[r][c+1] == piece && 
                board[r][c+2] == piece && board[r][c+3] == piece) return true;
        }
    }
    // Vertical
    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r < ROWS-3; r++) {
            if (board[r][c] == piece && board[r+1][c] == piece && 
                board[r+2][c] == piece && board[r+3][c] == piece) return true;
        }
    }
    // Diagonal
    for (int c = 0; c < COLS-3; c++) {
        for (int r = 0; r < ROWS-3; r++) {
            if (board[r][c] == piece && board[r+1][c+1] == piece && 
                board[r+2][c+2] == piece && board[r+3][c+3] == piece) return true;
        }
    }
    // Diagonal
    for (int c = 0; c < COLS-3; c++) {
        for (int r = 3; r < ROWS; r++) {
            if (board[r][c] == piece && board[r-1][c+1] == piece && 
                board[r-2][c+2] == piece && board[r-3][c+3] == piece) return true;
        }
    }
    return false;
}

bool isBoardFull() {
    for (int c = 0; c < COLS; c++) if (board[0][c] == 0) return false;
    return true;
}

int evaluate_window(int window[4], int piece) {
    int score = 0;
    int opp = (piece == 1) ? 2 : 1;

    int count_p = 0, count_e = 0, count_o = 0;
    for (int i = 0; i < 4; i++) {
        if (window[i] == piece) count_p++;
        else if (window[i] == 0) count_e++;
        else count_o++;
    }

    if (count_p == 4) score += 100;
    else if (count_p == 3 && count_e == 1) score += 5;
    else if (count_p == 2 && count_e == 2) score += 2;

    if (count_o == 3 && count_e == 1) score -= 4;

    return score;
}

int score_position(int piece) {
    int score = 0;

    // Center column
    for (int r = 0; r < ROWS; r++) {
        if (board[r][3] == piece) score += 3;
    }

    // Horizontal
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS-3; c++) {
            int window[4] = {board[r][c], board[r][c+1], board[r][c+2], board[r][c+3]};
            score += evaluate_window(window, piece);
        }
    }

    // Vertical
    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r < ROWS-3; r++) {
            int window[4] = {board[r][c], board[r+1][c], board[r+2][c], board[r+3][c]};
            score += evaluate_window(window, piece);
        }
    }

    // Diagonal
    for (int r = 0; r < ROWS-3; r++) {
        for (int c = 0; c < COLS-3; c++) {
            int window[4] = {board[r][c], board[r+1][c+1], board[r+2][c+2], board[r+3][c+3]};
            score += evaluate_window(window, piece);
        }
    }

    // Diagonal
    for (int r = 3; r < ROWS; r++) {
        for (int c = 0; c < COLS-3; c++) {
            int window[4] = {board[r][c], board[r-1][c+1], board[r-2][c+2], board[r-3][c+3]};
            score += evaluate_window(window, piece);
        }
    }

    return score;
};

int minimax(int depth, int alpha, int beta, bool maximizing) {
    if (depth == 0 || isBoardFull() || checkWin(1) || checkWin(2)) {
        if (checkWin(2)) return 100000000;
        if (checkWin(1)) return -100000000;
        if (isBoardFull()) return 0;
        return score_position(2);
    }

    if (maximizing) {
        int value = INT_MIN;
        for (int c = 0; c < COLS; c++) {
            if (isValidMove(c)) {
                int row = dropPiece(c, 2);
                int score = minimax(depth-1, alpha, beta, false);
                undoMove(c, row);
                if (score > value) value = score;
                alpha = (alpha > value) ? alpha : value;
                if (beta <= alpha) break;
            }
        }
        return value;
    } else {
        int value = INT_MAX;
        for (int c = 0; c < COLS; c++) {
            if (isValidMove(c)) {
                int row = dropPiece(c, 1);
                int score = minimax(depth-1, alpha, beta, true);
                undoMove(c, row);
                if (score < value) value = score;
                beta = (beta < value) ? beta : value;
                if (beta <= alpha) break;
            }
        }
        return value;
    }
}

int getBestAIMove() {
    int bestCol = -1;
    int bestScore = INT_MIN;

    for (int c = 0; c < COLS; c++) {
        if (isValidMove(c)) {
            int row = dropPiece(c, 2);
            int score = minimax(5, INT_MIN, INT_MAX, false);
            undoMove(c, row);

            if (score > bestScore) {
                bestScore = score;
                bestCol = c;
            }
        }
    }

    if (bestCol == -1) {
        do { bestCol = rand() % COLS; } while (!isValidMove(bestCol));
    }
    return bestCol;
}

void initGame() {
    gameTextBuf = C2D_TextBufNew(4096);
    C2D_TextParse(&textHumanTurn,  gameTextBuf, "YOUR TURN (Red)");
    C2D_TextParse(&textAITurn,     gameTextBuf, "AI THINKING... (Hard)");
    C2D_TextParse(&textWinRed,     gameTextBuf, "YOU WIN!");
    C2D_TextParse(&textWinYellow,  gameTextBuf, "AI WINS!");
    C2D_TextParse(&textDraw,       gameTextBuf, "DRAW!");
    C2D_TextParse(&textgControls,   gameTextBuf, "Controls:\n"
                                            "Left/Right or Joystick: select column\n"
                                            "A or tap bottom screen: drop piece\n"
                                            "SELECT : restart\n"
                                            "START  : exit");
}

void resetGameBoard() {
    memset(board, 0, sizeof(board));
    currentPlayer = 1;
    selectedCol = 3;
    gameState = STATE_PLAYING;
    isAIThinking = false;
    wasTouching = false;
}

void updateGame(u32 kDown, touchPosition touch) {
    if (gameState != STATE_PLAYING) return;

    if (currentPlayer == 1) { // Human
        // D-Pad - one column per press
        if (kDown & (KEY_LEFT | KEY_DLEFT))  selectedCol = (selectedCol + COLS - 1) % COLS;
        if (kDown & (KEY_RIGHT | KEY_DRIGHT)) selectedCol = (selectedCol + 1) % COLS;

        // Touch: hover while holding, drop on release
        bool isTouching = (touch.px != 0 || touch.py != 0);

        if (isTouching) {
            selectedCol = (touch.px * COLS) / BOTTOM_WIDTH;
            if (selectedCol < 0) selectedCol = 0;
            if (selectedCol >= COLS) selectedCol = COLS-1;
        } 
        else if (wasTouching) {
            int row = dropPiece(selectedCol, 1);
            if (row != -1) {
                if (checkWin(1)) gameState = STATE_WIN_RED;
                else if (isBoardFull()) gameState = STATE_DRAW;
                else {
                    currentPlayer = 2;
                    isAIThinking = true;
                }
            }
        }

        wasTouching = isTouching;   // this line stays here

        if (kDown & KEY_A) {
            int row = dropPiece(selectedCol, 1);
            if (row != -1) {
                if (checkWin(1)) gameState = STATE_WIN_RED;
                else if (isBoardFull()) gameState = STATE_DRAW;
                else {
                    currentPlayer = 2;
                    isAIThinking = true;
                }
            }
        }
    } else { // AI
        if (isAIThinking) {
            static int thinkFrame = 0;
            thinkFrame++;
            if (thinkFrame > 25) {
                thinkFrame = 0;
                isAIThinking = false;

                int aiCol = getBestAIMove();
                if (aiCol != -1) {
                    int row = dropPiece(aiCol, 2);
                    if (row != -1) {
                        if (checkWin(2)) gameState = STATE_WIN_YELLOW;
                        else if (isBoardFull()) gameState = STATE_DRAW;
                        else currentPlayer = 1;
                    }
                }
            }
        }
    }
}

void drawGameTop(C3D_RenderTarget* top) {
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

    if (currentPlayer == 1 && gameState == STATE_PLAYING) {
        float ax = BOARD_X + selectedCol * CELL_SIZE + CELL_SIZE/2.0f;
        float ay = BOARD_Y - 22.0f;
        C2D_DrawTriangle(ax-12, ay+12, COLOR_HIGHLIGHT,
                         ax+12, ay+12, COLOR_HIGHLIGHT,
                         ax,    ay,     COLOR_HIGHLIGHT, 0.0f);
    }

    if (gameState == STATE_WIN_RED) {
        C2D_DrawRectSolid(0, 90, 0.0f, 400, 60, C2D_Color32(220, 20, 20, 100));   // red strip
        C2D_DrawText(&textWinRed, C2D_AlignCenter, 200, 100, 0.0f, 2.0f, 2.0f);
    } else if (gameState == STATE_WIN_YELLOW) {
        C2D_DrawRectSolid(0, 90, 0.0f, 400, 60, C2D_Color32(255, 215, 0, 100));   // yellow strip
        C2D_DrawText(&textWinYellow, C2D_AlignCenter, 200, 100, 0.0f, 2.0f, 2.0f);
    } else if (gameState == STATE_DRAW) {
        C2D_DrawRectSolid(0, 90, 0.0f, 400, 60, C2D_Color32(100, 100, 100, 100));   // gray strip
        C2D_DrawText(&textDraw, C2D_AlignCenter, 200, 100, 0.0f, 2.0f, 2.0f);
    }
}

void drawGameBottom(C3D_RenderTarget* bottom) {
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(124, 118, 106, 255));
    // Turn indicator strip - 20% of bottom screen height (≈48px tall)
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
        C2D_Text* t = (currentPlayer == 1) ? &textHumanTurn : &textAITurn;
        C2D_DrawText(t, C2D_AlignCenter, 160, 20, 0.0f, 0.8f, 0.8f);
    }

    C2D_DrawText(&textgControls, 0, 15, 65, 0.0f, 0.65f, 0.65f);
}

bool isGameOver() {
    return gameState != STATE_PLAYING;
}