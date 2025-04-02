#pragma once
#include "../../HUDModule.h"
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

struct Tetromino {
    int shape[4][4];
    d2d::Color color;
    int dimension;
    int type;  // 0=I,1=J,2=L,3=O,4=S,5=T,6=Z
    int rotationState = 0;
};

class BlockGame : public Module {
public:
    BlockGame();

    void onRenderOverlay(Event&);
    void onRenderHUDModules(Event& ev);
    void onKeyUpdate(Event&);
    void restartGame();

private:
    enum RotationState { STATE_0, STATE_R, STATE_2, STATE_L };

    static const int BOARD_WIDTH = 10;
    static const int BOARD_HEIGHT = 20;
    static const int NEXT_SIZE = 4;
    static const int HOLD_SIZE = 4;

    // Official SRS offset data
    const std::array<std::array<Vec2, 5>, 4> JLSTZ_OFFSETS = { {
        // STATE_0
        { Vec2{0,0}, Vec2{0,0}, Vec2{0,0}, Vec2{0,0}, Vec2{0,0} },
        // STATE_R
        { Vec2{0,0}, Vec2{1,0}, Vec2{1,-1}, Vec2{0,2}, Vec2{1,2} },
        // STATE_2
        { Vec2{0,0}, Vec2{0,0}, Vec2{0,0}, Vec2{0,0}, Vec2{0,0} },
        // STATE_L
        { Vec2{0,0}, Vec2{-1,0}, Vec2{-1,-1}, Vec2{0,2}, Vec2{-1,2} }
    } };

    const std::array<std::array<Vec2, 5>, 4> I_OFFSETS = { {
        // STATE_0
        { Vec2{0,0}, Vec2{-1,0}, Vec2{2,0}, Vec2{-1,0}, Vec2{2,0} },
        // STATE_R
        { Vec2{-1,0}, Vec2{0,0}, Vec2{0,0}, Vec2{0,1}, Vec2{0,-2} },
        // STATE_2
        { Vec2{-1,1}, Vec2{1,1}, Vec2{-2,1}, Vec2{1,0}, Vec2{-2,0} },
        // STATE_L
        { Vec2{0,1}, Vec2{0,1}, Vec2{0,1}, Vec2{0,-1}, Vec2{0,2} }
    } };

    const std::array<Vec2, 4> O_OFFSETS = {
        Vec2{0,0},  // STATE_0
        Vec2{0,-1}, // STATE_R
        Vec2{-1,-1},// STATE_2
        Vec2{-1,0}  // STATE_L
    };

    std::vector<std::vector<int>> board;
    std::vector<std::vector<int>> nextBoard;
    std::vector<std::vector<int>> holdBoard;
    Tetromino currentTetromino;
    Tetromino nextTetromino;
    Tetromino holdTetromino;

    std::vector<int> tetrominoBag;
    bool hasHold = false;
    bool canHold = true;
    bool lastWasRotation = false;
    bool lastMoveWasTSpin = false;
    bool lastMoveWasMiniTSpin = false;

    int score;
    int level;
    int linesCleared;
    Vec2 piecePosition;
    Vec2 lastKickOffset = { 0,0 };
    bool gameOver;
    bool paused;
    bool softDropActive = false;
    int softDropScore = 0;

    std::chrono::time_point<std::chrono::steady_clock> lastFall;
    std::vector<Tetromino> tetrominoShapes;
    std::chrono::steady_clock::time_point gameOverTime;

    // SRS kick tables
    const std::vector<std::vector<Vec2>> JLSTZ_KICKS = {
        {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}}, // 0->R
        {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}}, // R->2
        {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}}, // 2->L
        {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}} // L->0
    };

    const std::vector<std::vector<Vec2>> I_KICKS = {
        {{0,0}, {-2,0}, {1,0}, {-2,-1}, {1,2}}, // 0->R
        {{0,0}, {-1,0}, {2,0}, {-1,2}, {2,-1}}, // R->2
        {{0,0}, {2,0}, {-1,0}, {2,1}, {-1,-2}}, // 2->L
        {{0,0}, {1,0}, {-2,0}, {1,-2}, {-2,1}} // L->0
    };

    void createTetrominoShapes();
    void spawnTetromino(bool firstSpawn);
    bool isValidMove(const Tetromino& tetro, Vec2 pos);
    void mergeTetromino();
    void clearLines();
    void rotateTetromino(bool clockwise = true);
    Tetromino rotateMatrix(const Tetromino& original, bool clockwise);
    void moveHorizontal(int dx);
    void hardDrop();
    void handleHold();
    std::vector<Vec2> getKicks(int type, int fromState, int toState);

    ValueType audio = BoolValue(true);
    ValueType leftKey = KeyValue(VK_LEFT);
    ValueType rightKey = KeyValue(VK_RIGHT);
    ValueType hardDropKey = KeyValue(VK_SPACE);
    ValueType softDropKey = KeyValue(VK_DOWN);
    ValueType rotateKey = KeyValue(VK_UP);
    ValueType rotateCCWKey = KeyValue('Z');
    ValueType holdKey = KeyValue('H');
    ValueType pauseKey = KeyValue(VK_INSERT);
    ValueType restartKey = KeyValue(VK_DELETE);
};
