#pragma once
#include "../../HUDModule.h"
#include <vector>
#include <chrono>
#include <random>
#include <array>

struct Tetromino {
    int shape[4][4];
    d2d::Color color;
    int dimension;
    int type;
    int rotationState = 0;
};

class BlockGame : public Module {
public:
    BlockGame();

    void onEnable() override;
    void onDisable() override;
    void onRenderOverlay(Event&);
    void onRenderHUDModules(Event& ev);
    void onKey(Event&);
    void restartGame();

private:
    static const int BOARD_WIDTH = 10;
    static const int BOARD_HEIGHT = 20;
    static const int BOARD_BUFFER = 2;
    static const int TOTAL_BOARD_HEIGHT = BOARD_HEIGHT + BOARD_BUFFER;
    static const int NEXT_SIZE = 4;
    static const int HOLD_SIZE = 4;

    enum RotationState { STATE_0, STATE_R, STATE_2, STATE_L };
    enum TetrominoType { I_PIECE, J_PIECE, L_PIECE, O_PIECE, S_PIECE, T_PIECE, Z_PIECE };

    std::vector<std::vector<int>> board;
    std::vector<Tetromino> tetrominoShapes;
    Tetromino currentTetromino;
    Tetromino nextTetromino;
    Tetromino holdTetromino;
    Vec2 piecePosition;
    bool gameOver = false;
    bool paused = false;
    int score = 0;
    int level = 1;
    int linesCleared = 0;
    int comboCount = 0;
    int b2bCount = 0;
    std::vector<int> tetrominoBag;
    std::chrono::steady_clock::time_point lastFall;
    std::chrono::steady_clock::time_point gameOverTime;

    bool hasHold = false;
    bool canHold = true;

    bool isLocking = false;
    std::chrono::steady_clock::time_point lockStartTime;
    int lockResets = 0;
    const int MAX_LOCK_RESETS = 15;

    bool lastActionWasRotation = false;
    Vec2 lastKickOffset = {0, 0};
    bool lastMoveWasTSpin = false;
    bool lastMoveWasMiniTSpin = false;
    bool lastClearWasDifficult = false;

    enum class DasState { IDLE, CHARGING, REPEATING };

    DasState dasStateLeft = DasState::IDLE;
    DasState dasStateRight = DasState::IDLE;
    std::chrono::steady_clock::time_point dasStartTimeLeft;
    std::chrono::steady_clock::time_point dasStartTimeRight;
    std::chrono::steady_clock::time_point arrTimeLeft;
    std::chrono::steady_clock::time_point arrTimeRight;
    std::chrono::steady_clock::time_point dcdEndTimeLeft;
    std::chrono::steady_clock::time_point dcdEndTimeRight;

    std::chrono::steady_clock::time_point comboUpdateTime;
    std::chrono::steady_clock::time_point b2bUpdateTime;
    const std::chrono::milliseconds displayFlashDuration{300};

    bool leftKeyHeld = false;
    bool rightKeyHeld = false;
    bool softDropKeyHeld = false;
    std::chrono::steady_clock::time_point softDropArrTime;

    using KickSet = std::array<Vec2, 5>;
    using KickTable = std::array<KickSet, 8>;

    const KickTable srsKickDataJLSTZ = {
        {
            // 0->R (Clockwise from 0)
            {{{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}},
            // R->0 (Counter-Clockwise from R)
            {{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}},
            // R->2 (Clockwise from R)
            {{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}},
            // 2->R (Counter-Clockwise from 2)
            {{{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}},
            // 2->L (Clockwise from 2)
            {{{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}},
            // L->2 (Counter-Clockwise from L)
            {{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}},
            // L->0 (Clockwise from L)
            {{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}},
            // 0->L (Counter-Clockwise from 0)
            {{{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}}
        }
    };

    const KickTable srsKickDataI = {
        {
            // 0->R
            { {{0 , 0 }, { -2, 0 }, { 1, 0 }, { -2, -1 }, { 1, 2 } } },
            // R->0
            {{{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}},
            // R->2
            {{{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}},
            // 2->R
            {{{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}},
            // 2->L
            {{{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}},
            // L->2
            {{{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}},
            // L->0
            {{{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}},
            // 0->L
            {{{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}}
        }
    };

    // no kicks for O piece
    const KickTable srsKickDataO = {
        {
            {{{0, 0}}}, {{{0, 0}}}, {{{0, 0}}}, {{{0, 0}}},
            {{{0, 0}}}, {{{0, 0}}}, {{{0, 0}}}, {{{0, 0}}}
        }
    };

    // some basic kicks i stole from somewhere for 180
    const KickTable kicks180 = {
        {
            {{{0, 0}}},
            {{{1, 0}}},
            {{{-1, 0}}},
            {{{0, 1}}},
            {{{0, -1}}}
        }
    };

    void createTetrominoShapes();
    void spawnTetromino(bool firstSpawn);
    bool isValidPosition(const Tetromino& tetro, Vec2 pos);
    bool testMove(int dx, int dy);
    void applyMove(int dx, int dy);
    void hardDrop();
    void mergeTetromino();
    int clearLinesAndScore(bool tspin, bool miniTspin);
    Tetromino getRotatedTetromino(const Tetromino& original, bool clockwise);
    Tetromino get180RotatedTetromino(const Tetromino& original);
    bool rotateTetromino(bool clockwise);
    bool handle180Rotation();
    void handleHold();
    bool checkTSpinCorners();
    void updateGameLogic(std::chrono::steady_clock::time_point now);
    void handleInput(std::chrono::steady_clock::time_point now);
    void tryMoveHorizontal(int dx);

    ValueType audio = BoolValue(true);
    ValueType backgroundEnabled = BoolValue(false);
    ValueType drawGrid = BoolValue(true);
    ValueType backgroundColor = ColorValue{ 1.f, 0.f, 0.f, 1.f };
    ValueType leftKey = KeyValue(VK_LEFT);
    ValueType rightKey = KeyValue(VK_RIGHT);
    ValueType hardDropKey = KeyValue(VK_SPACE);
    ValueType softDropKey = KeyValue(VK_DOWN);
    ValueType dasDelaySetting = FloatValue(130.f);
    ValueType arrIntervalSetting = FloatValue(20.f);
    ValueType dcdDelaySetting = FloatValue(60.f);
    ValueType sdfSetting = FloatValue(20.f);
    ValueType rotateKey = KeyValue(VK_UP);
    ValueType rotateCCWKey = KeyValue('Z');
    ValueType rotate180Key = KeyValue('A');
    ValueType holdKey = KeyValue('C');
    ValueType pauseKey = KeyValue(VK_INSERT);
    ValueType restartKey = KeyValue(VK_DELETE);

    bool isKeyDown(int vkCode);
    void playSound(const std::string& soundId);
};
