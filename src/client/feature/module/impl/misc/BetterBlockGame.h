#pragma once
#include "../../HUDModule.h"
#include <vector>
#include <chrono>

struct Tetromino {
    int shape[4][4];
    d2d::Color color;
    int dimension;
};

class BetterBlockGame : public Module {
public:
    BetterBlockGame();

    void onRenderOverlay(Event&);
    void onRenderHUDModules(Event& ev);
    void onKeyUpdate(Event&);
    void restartGame();

private:
    static const int BOARD_WIDTH = 10;
    static const int BOARD_HEIGHT = 20;
    static const int NEXT_SIZE = 4;

    std::vector<std::vector<int>> board;
    std::vector<std::vector<int>> nextBoard;
    Tetromino currentTetromino;
    Tetromino nextTetromino;

    int score;
    int level;
    int linesCleared;
    Vec2 piecePosition;
    bool gameOver;
    bool paused;
    bool softDropActive = false;
    int softDropScore = 0;

    std::chrono::time_point<std::chrono::steady_clock> lastFall;
    std::vector<Tetromino> tetrominoShapes;
    std::chrono::steady_clock::time_point gameOverTime;

    void createTetrominoShapes();
    void spawnTetromino();
    bool isValidMove(const Tetromino& tetro, Vec2 pos);
    void mergeTetromino();
    void clearLines();
    void rotateTetromino();
    void moveHorizontal(int dx);
    void hardDrop();

    ValueType leftKey = KeyValue(VK_LEFT);
    ValueType rightKey = KeyValue(VK_RIGHT);
    ValueType hardDropKey = KeyValue(VK_SPACE);
    ValueType softDropKey = KeyValue(VK_DOWN);
    ValueType rotateKey = KeyValue(VK_UP);
    ValueType pauseKey = KeyValue(VK_INSERT);
    ValueType restartKey = KeyValue(VK_DELETE);
};