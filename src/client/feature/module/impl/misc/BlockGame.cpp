#include "pch.h"
#include "BlockGame.h"
#include "client/Latite.h"
#include "client/render/Assets.h"

BlockGame::BlockGame() : Module("BlockGame", L"Block Game",
                                            L"The better one", HUD, nokeybind) {
    listen<RenderOverlayEvent>(static_cast<EventListenerFunc>(&BlockGame::onRenderOverlay));
    listen<KeyUpdateEvent>(static_cast<EventListenerFunc>(&BlockGame::onKeyUpdate));
    listen<DrawHUDModulesEvent>(static_cast<EventListenerFunc>(&BlockGame::onRenderHUDModules), false, 2);

    addSetting("moveLeftSetting", L"Left",
               L"Key to move left", leftKey);
    addSetting("moveRightSetting", L"Right",
               L"Key to move right", rightKey);
    addSetting("hardDropSetting", L"Hard drop",
               L"Key to instantly drop a piece", hardDropKey);
    addSetting("softDropSetting", L"Soft drop",
        L"Key to slowly drop a piece", softDropKey);
    addSetting("rotateSetting", L"Rotate",
               L"Rotate key", rotateKey);
    addSetting("pauseSetting", L"Pause",
               L"Pause key", pauseKey);
    addSetting("restartSetting", L"Restart",
        L"Restart game key", restartKey);

    createTetrominoShapes();
    restartGame();
}

void BlockGame::onRenderOverlay(Event& evG) {
    RenderOverlayEvent& ev = reinterpret_cast<RenderOverlayEvent&>(evG);

    if (!SDK::ClientInstance::get()->getLocalPlayer() || !SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) return;
    D2DUtil dc;

    // I HATE SIZE CALCULATIONS!!! I HATE THIS!!!
    auto [width, height] = Latite::getRenderer().getScreenSize();

    const float blockSize = width * 0.025f;
    const float boardLeft = width * 0.55f;
    const float boardHeight = BOARD_HEIGHT * blockSize;
    const float boardTop = (height - boardHeight) / 2.0f;

    float nextLeft = boardLeft + BOARD_WIDTH * blockSize + 20;
    float nextTop = boardTop;

    if (paused) {
        // just chose a random place for this honestly cant find a better place to put it
        dc.drawText({ 0, 0, 300, 100 },
            L"Paused",
            d2d::Colors::WHITE,
            Renderer::FontSelection::PrimaryRegular,
            48.0f,
            DWRITE_TEXT_ALIGNMENT_CENTER
        );
        return;
    }

    if (gameOver) {
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - gameOverTime).count();
        int remaining = 5 - elapsed;

        std::wstring statusText = L"Game Over!";
        std::wstring countdown = L"Restarting in " + std::to_wstring(remaining) + L" seconds...";

        if (remaining <= 0) {
            restartGame();
            return;
        }

        // score n other shit
        std::wstring stats = L"Score: " + std::to_wstring(score) + L"\n"
            L"Level: " + std::to_wstring(level) + L"\n"
            L"Lines: " + std::to_wstring(linesCleared);
        dc.drawText({ nextLeft, nextTop + 150, nextLeft + 200, nextTop + 250 },
            stats, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular);

        dc.drawText({ 0, 0, 300, 100 },
            statusText,
            d2d::Colors::WHITE,
            Renderer::FontSelection::PrimaryRegular,
            48.0f,
            DWRITE_TEXT_ALIGNMENT_CENTER
        );

        dc.drawText({ 0, 120, 300, 160 },
            countdown,
            d2d::Colors::YELLOW,
            Renderer::FontSelection::PrimaryRegular,
            24.0f,
            DWRITE_TEXT_ALIGNMENT_CENTER
        );
        return;
    }

    const float borderThickness = 2.0f;
    dc.drawRectangle(
        {
            boardLeft - borderThickness,
            boardTop - borderThickness,
            boardLeft + BOARD_WIDTH * blockSize + borderThickness,
            boardTop + BOARD_HEIGHT * blockSize + borderThickness
        },
        d2d::Colors::WHITE,
        borderThickness
    );

    // game board
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x]) {
                dc.fillRectangle(
                    {
                        boardLeft + x * blockSize, boardTop + y * blockSize,
                        boardLeft + (x + 1) * blockSize, boardTop + (y + 1) * blockSize
                    },
                    tetrominoShapes[board[y][x] - 1].color
                );
            }
        }
    }

    // current piece
    for (int y = 0; y < currentTetromino.dimension; y++) {
        for (int x = 0; x < currentTetromino.dimension; x++) {
            if (currentTetromino.shape[y][x]) {
                dc.fillRectangle(
                    {
                        boardLeft + (piecePosition.x + x) * blockSize,
                        boardTop + (piecePosition.y + y) * blockSize,
                        boardLeft + (piecePosition.x + x + 1) * blockSize,
                        boardTop + (piecePosition.y + y + 1) * blockSize
                    },
                    currentTetromino.color
                );
            }
        }
    }

    // next piece preview
    dc.drawText({ nextLeft, nextTop, nextLeft + 100, nextTop + 30 },
        L"Next:", d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular);

    for (int y = 0; y < nextTetromino.dimension; y++) {
        for (int x = 0; x < nextTetromino.dimension; x++) {
            if (nextTetromino.shape[y][x]) {
                dc.fillRectangle(
                    {
                        nextLeft + x * blockSize, nextTop + 40 + y * blockSize,
                        nextLeft + (x + 1) * blockSize, nextTop + 40 + (y + 1) * blockSize
                    },
                    nextTetromino.color
                );
            }
        }
    }

    // score n other shit
    std::wstring stats = L"Score: " + std::to_wstring(score) + L"\n"
        L"Level: " + std::to_wstring(level) + L"\n"
        L"Lines: " + std::to_wstring(linesCleared);
    dc.drawText({ nextLeft, nextTop + 150, nextLeft + 200, nextTop + 250 },
        stats, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular);

    // auto fall
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    std::chrono::milliseconds baseDelay = std::chrono::milliseconds(1000 - (level - 1) * 100);
    auto fallDelay = softDropActive ? baseDelay / 20 : baseDelay;

    if (now - lastFall > fallDelay) {
        if (isValidMove(currentTetromino, piecePosition + Vec2{ 0, 1 })) {
            piecePosition.y++;
            if (softDropActive) {
                softDropScore += level;
            }
        }
        else {
            score += softDropScore;
            softDropScore = 0;
            mergeTetromino();
            clearLines();
            spawnTetromino();
        }
        lastFall = now;
    }

    // soft drop score preview
    if (softDropActive) {
        std::wstring dropScore = L"+" + std::to_wstring(softDropScore);
        dc.drawText({ nextLeft, nextTop + 300, nextLeft + 350, nextTop + 400 },
            dropScore, d2d::Colors::GREEN, Renderer::FontSelection::PrimaryRegular);
    }

}

void BlockGame::onKeyUpdate(Event& evG) {
    KeyUpdateEvent& ev = reinterpret_cast<KeyUpdateEvent&>(evG);
    if (!ev.isDown()) {
        softDropActive = false;
        return;
    }

    if (ev.getKey() == std::get<KeyValue>(pauseKey) && ev.isDown()) {
        paused = !paused;
        return;
    }

    KeyValue key = ev.getKey();

    if (gameOver || paused) return;

    if (ev.getKey() == std::get<KeyValue>(softDropKey)) {
        softDropActive = true;
    }

    if (key == std::get<KeyValue>(leftKey)) {
        moveHorizontal(-1);
    } else if (key == std::get<KeyValue>(rightKey)) {
        moveHorizontal(1);
    } else if (key == std::get<KeyValue>(hardDropKey)) {
        hardDrop();
    } else if (key == std::get<KeyValue>(rotateKey)) {
        rotateTetromino();
    } else if (key == std::get<KeyValue>(restartKey)) {
        restartGame();
    }
}

void BlockGame::createTetrominoShapes() {
    tetrominoShapes = {
        // I-shape
        { {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, d2d::Color::RGB(0, 240, 240), 4 },
        // J-shape
        { {{2, 0, 0}, {2, 2, 2}, {0, 0, 0}}, d2d::Color::RGB(0, 0, 240), 3 },
        // L-shape
        { {{0, 0, 3}, {3, 3, 3}, {0, 0, 0}}, d2d::Color::RGB(240, 160, 0), 3 },
        // O-shape
        { {{4, 4}, {4, 4}}, d2d::Color::RGB(240, 240, 0), 2 },
        // S-shape
        { {{0, 5, 5}, {5, 5, 0}}, d2d::Color::RGB(0, 240, 0), 3 },
        // T-shape
        { {{0, 6, 0}, {6, 6, 6}}, d2d::Color::RGB(160, 0, 240), 3 },
        // Z-shape
        { {{7, 7, 0}, {0, 7, 7}}, d2d::Color::RGB(240, 0, 0), 3 }
    };

    if (tetrominoShapes.empty()) {
        Logger::Fatal("Failed to initialize tetromino shapes");
    }
}

void BlockGame::spawnTetromino() {
    if (tetrominoShapes.empty()) {
        createTetrominoShapes();
    }

    if (nextTetromino.dimension > 0 && nextTetromino.dimension <= 4) {
        currentTetromino = nextTetromino;
    } else {
        if (!tetrominoShapes.empty()) {
            currentTetromino = tetrominoShapes[0];
        } else {
            Logger::Fatal("No tetromino shapes available");
        }
    }

    if (!tetrominoShapes.empty()) {
        nextTetromino = tetrominoShapes[rand() % tetrominoShapes.size()];

        // Initialize next board
        const int offset = (NEXT_SIZE - nextTetromino.dimension) / 2;
        for (int y = 0; y < nextTetromino.dimension; y++) {
            for (int x = 0; x < nextTetromino.dimension; x++) {
                if (y + offset < NEXT_SIZE && x + offset < NEXT_SIZE) {
                    nextBoard[y + offset][x + offset] = nextTetromino.shape[y][x];
                }
            }
        }
    }

    piecePosition = {
        static_cast<float>(BOARD_WIDTH / 2 - currentTetromino.dimension / 2),
        0
    };

    if (!isValidMove(currentTetromino, piecePosition)) {
        gameOver = true;
        gameOverTime = std::chrono::steady_clock::now();
    }
}

void BlockGame::restartGame() {
    board = std::vector(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));
    nextBoard = std::vector(NEXT_SIZE, std::vector<int>(NEXT_SIZE, 0));
    score = 0;
    level = 1;
    linesCleared = 0;
    gameOver = false;
    paused = false;
    spawnTetromino();
    lastFall = std::chrono::steady_clock::now();
    gameOverTime = std::chrono::steady_clock::time_point::min();
}

bool BlockGame::isValidMove(const Tetromino& tetro, Vec2 pos) {
    for (int y = 0; y < tetro.dimension; y++) {
        for (int x = 0; x < tetro.dimension; x++) {
            if (tetro.shape[y][x]) {
                int boardX = pos.x + x;
                int boardY = pos.y + y;
                if (boardX < 0 || boardX >= BOARD_WIDTH ||
                    boardY >= BOARD_HEIGHT ||
                    (boardY >= 0 && board[boardY][boardX])) {
                    return false;
                }
            }
        }
    }
    return true;
}

void BlockGame::moveHorizontal(int dx) {
    Vec2 newPos = piecePosition;
    newPos.x += dx;
    if (isValidMove(currentTetromino, newPos)) {
        piecePosition = newPos;
    }
}

void BlockGame::rotateTetromino() {
    Tetromino rotated = currentTetromino;
    for (int y = 0; y < rotated.dimension; y++) {
        for (int x = 0; x < rotated.dimension; x++) {
            rotated.shape[y][x] = currentTetromino.shape[rotated.dimension - x - 1][y];
        }
    }

    if (isValidMove(rotated, piecePosition)) {
        currentTetromino = rotated;
    }
}

void BlockGame::hardDrop() {
    int dropDistance = 0;
    while (isValidMove(currentTetromino, piecePosition + Vec2{ 0, 1 })) {
        piecePosition.y++;
        dropDistance++;
    }
    score += dropDistance * 2 * level;
    mergeTetromino();
    clearLines();
    spawnTetromino();
    softDropScore = 0;
}

void BlockGame::mergeTetromino() {
    for (int y = 0; y < currentTetromino.dimension; y++) {
        for (int x = 0; x < currentTetromino.dimension; x++) {
            if (currentTetromino.shape[y][x]) {
                int boardY = piecePosition.y + y;
                if (boardY >= 0) {
                    board[boardY][piecePosition.x + x] = currentTetromino.shape[y][x];
                }
            }
        }
    }
}

void BlockGame::clearLines() {
    int linesClearedThisTurn = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!board[y][x]) {
                full = false;
                break;
            }
        }

        if (full) {
            board.erase(board.begin() + y);
            board.insert(board.begin(), std::vector(BOARD_WIDTH, 0));
            linesClearedThisTurn++;
            y++;
        }
    }

    if (linesClearedThisTurn > 0) {
        linesCleared += linesClearedThisTurn;
        score += (linesClearedThisTurn * 100) * level;
        if (linesCleared >= level * 10) {
            level++;
        }
    }
}

void BlockGame::onRenderHUDModules(Event& evGeneric) {
    DrawHUDModulesEvent& ev = reinterpret_cast<DrawHUDModulesEvent&>(evGeneric);
    ev.setCancelled(true);
}
