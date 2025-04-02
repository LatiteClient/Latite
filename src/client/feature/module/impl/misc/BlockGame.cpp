#include "pch.h"
#include "BlockGame.h"
#include "client/Latite.h"
#include "client/render/Assets.h"

BlockGame::BlockGame() : Module("BlockGame", LocalizeString::get("client.module.blockGame.name"),
                                LocalizeString::get("client.module.blockGame.desc"), HUD, nokeybind) {
    listen<RenderOverlayEvent>(static_cast<EventListenerFunc>(&BlockGame::onRenderOverlay));
    listen<KeyUpdateEvent>(static_cast<EventListenerFunc>(&BlockGame::onKeyUpdate));
    listen<DrawHUDModulesEvent>(static_cast<EventListenerFunc>(&BlockGame::onRenderHUDModules), false, 2);

    addSetting("audioSetting", LocalizeString::get("client.module.blockGame.audioSetting.name"),
               LocalizeString::get("client.module.blockGame.audioSetting.desc"), audio);
    addSetting("moveLeftSetting", LocalizeString::get("client.module.blockGame.moveLeftSetting.name"),
               LocalizeString::get("client.module.blockGame.moveLeftSetting.desc"), leftKey);
    addSetting("moveRightSetting", LocalizeString::get("client.module.blockGame.moveRightSetting.name"),
               LocalizeString::get("client.module.blockGame.moveRightSetting.desc"), rightKey);
    addSetting("hardDropSetting", LocalizeString::get("client.module.blockGame.hardDropSetting.name"),
               LocalizeString::get("client.module.blockGame.hardDropSetting.desc"), hardDropKey);
    addSetting("softDropSetting", LocalizeString::get("client.module.blockGame.softDropSetting.name"),
               LocalizeString::get("client.module.blockGame.softDropSetting.desc"), softDropKey);
    addSetting("rotateSetting", LocalizeString::get("client.module.blockGame.rotateSetting.name"),
               LocalizeString::get("client.module.blockGame.rotateSetting.desc"), rotateKey);
    addSetting("rotateCCWSetting", LocalizeString::get("client.module.blockGame.rotateCCWSetting.name"),
        LocalizeString::get("client.module.blockGame.rotateCCWSetting.desc"), rotateCCWKey);
    addSetting("rotate180Setting", LocalizeString::get("client.module.blockGame.rotate180Setting.name"),
        LocalizeString::get("client.module.blockGame.rotate180Setting.desc"), rotateCCWKey);
    addSetting("pauseSetting", LocalizeString::get("client.module.blockGame.pauseSetting.name"),
               LocalizeString::get("client.module.blockGame.pauseSetting.desc"), pauseKey);
    addSetting("restartSetting", LocalizeString::get("client.module.blockGame.restartSetting.name"),
               LocalizeString::get("client.module.blockGame.restartSetting.desc"), restartKey);
    addSetting("holdSetting", LocalizeString::get("client.module.blockGame.holdSetting.name"),
               LocalizeString::get("client.module.blockGame.holdSetting.desc"), holdKey);

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
    const float boardWidth = BOARD_WIDTH * blockSize;
    const float boardTop = (height - boardHeight) / 2.0f;

    float nextLeft = boardLeft + BOARD_WIDTH * blockSize + 20;
    float nextTop = boardTop;

    if (paused) {
        // just chose a random place for this honestly cant find a better place to put it
        dc.drawText({ 0, 0, 300, 100 },
                    LocalizeString::get("client.module.blockGame.pausedText.name"),
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

        std::wstring statusText = LocalizeString::get("client.module.blockGame.gameOverText.name");
        std::wstring countdown = util::FormatWString(
            LocalizeString::get("client.module.blockGame.restartCountdown.name"), { std::to_wstring(remaining) });

        if (remaining <= 0) {
            if (std::get<BoolValue>(audio)) {
                util::PlaySoundUI("note.snare");
            }
            restartGame();
            return;
        }

        // score n other shit
        std::wstring stats = util::FormatWString(LocalizeString::get("client.module.blockGame.scoreText.name"),
                                                 { std::to_wstring(score) }) + L"\n" + util::FormatWString(
            LocalizeString::get("client.module.blockGame.levelText.name"),
            { std::to_wstring(level) }) + L"\n" + util::FormatWString(
            LocalizeString::get("client.module.blockGame.linesClearedText.name"), { std::to_wstring(linesCleared) });
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

    // game border
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

    // grid lines
    const float gridAlpha = 0.2f;
    const auto gridColor = d2d::Color::Hex("FFFFFF", gridAlpha);
    const float lineThickness = 0.5f;

    for (int x = 0; x < BOARD_WIDTH; x++) {
        float xPos = boardLeft + x * blockSize;
        dc.fillRectangle(
            { xPos - lineThickness, boardTop,
                xPos + lineThickness, boardTop + boardHeight },
            gridColor
        );
    }

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        float yPos = boardTop + y * blockSize;
        dc.fillRectangle(
            { boardLeft, yPos - lineThickness,
                boardLeft + boardWidth, yPos + lineThickness },
            gridColor
        );
    }

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

    // hold piece display
    float holdLeft = boardLeft - 150;
    float holdTop = boardTop;
    dc.drawText({ holdLeft, holdTop, holdLeft + 100, holdTop + 30 },
                LocalizeString::get("client.module.blockGame.holdPieceText.name"), d2d::Colors::WHITE,
                Renderer::FontSelection::PrimaryRegular);

    if (hasHold) {
        for (int y = 0; y < holdTetromino.dimension; y++) {
            for (int x = 0; x < holdTetromino.dimension; x++) {
                if (holdTetromino.shape[y][x]) {
                    dc.fillRectangle(
                        {
                            holdLeft + x * blockSize, holdTop + 40 + y * blockSize,
                            holdLeft + (x + 1) * blockSize, holdTop + 40 + (y + 1) * blockSize
                        },
                        holdTetromino.color
                    );
                }
            }
        }
    }

    // next piece preview
    dc.drawText({ nextLeft, nextTop, nextLeft + 100, nextTop + 30 },
                LocalizeString::get("client.module.blockGame.nextPiecePreviewText.name"), d2d::Colors::WHITE,
                Renderer::FontSelection::PrimaryRegular);

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
    std::wstring stats = util::FormatWString(LocalizeString::get("client.module.blockGame.scoreText.name"),
                                             { std::to_wstring(score) }) + L"\n" + util::FormatWString(
        LocalizeString::get("client.module.blockGame.levelText.name"),
        { std::to_wstring(level) }) + L"\n" + util::FormatWString(
        LocalizeString::get("client.module.blockGame.linesClearedText.name"), { std::to_wstring(linesCleared) });
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
            spawnTetromino(false);
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
        if (std::get<BoolValue>(audio)) {
            util::PlaySoundUI("note.snare");
        }
        restartGame();
    } else if (key == std::get<KeyValue>(holdKey)) {
        if (canHold) handleHold();
    } else if (key == std::get<KeyValue>(rotateCCWKey)) {
        rotateTetromino(false);
    } else if (key == 65) {
        rotateTetromino(false, true);
    }
}

void BlockGame::handleHold() {
    if (!hasHold) {
        holdTetromino = tetrominoShapes[currentTetromino.type];
        holdTetromino.rotationState = STATE_0;
        hasHold = true;
        spawnTetromino(false);
    }
    else {
        Tetromino temp = holdTetromino;
        temp.rotationState = STATE_0;
        holdTetromino = currentTetromino;
        holdTetromino.rotationState = STATE_0;

        currentTetromino = temp;
        currentTetromino.rotationState = STATE_0;

        piecePosition = {
            static_cast<float>(BOARD_WIDTH / 2 - currentTetromino.dimension / 2),
            0
        };

        if (!isValidMove(currentTetromino, piecePosition)) {
            gameOver = true;
            gameOverTime = std::chrono::steady_clock::now();
        }
    }
    canHold = false;
}

void BlockGame::createTetrominoShapes() {
    tetrominoShapes = {
        // I-shape (type 0)
        { {{1,1,1,1},{0,0,0,0},{0,0,0,0},{0,0,0,0}}, d2d::Color::RGB(0,240,240), 4, 0, STATE_0 },
        // J-shape (type 1)
        { {{2,0,0},{2,2,2},{0,0,0}}, d2d::Color::RGB(0,0,240), 3, 1, STATE_0 },
        // L-shape (type 2)
        { {{0,0,3},{3,3,3},{0,0,0}}, d2d::Color::RGB(240,160,0), 3, 2, STATE_0 },
        // O-shape (type 3)
        { {{4,4},{4,4}}, d2d::Color::RGB(240,240,0), 2, 3, STATE_0 },
        // S-shape (type 4)
        { {{0,5,5},{5,5,0}}, d2d::Color::RGB(0,240,0), 3, 4, STATE_0 },
        // T-shape (type 5)
        { {{0,6,0},{6,6,6}}, d2d::Color::RGB(160,0,240), 3, 5, STATE_0 },
        // Z-shape (type 6)
        { {{7,7,0},{0,7,7}}, d2d::Color::RGB(240,0,0), 3, 6, STATE_0 }
    };

    if (tetrominoShapes.empty()) {
        Logger::Fatal("Failed to initialize tetromino shapes");
    }
}


void BlockGame::spawnTetromino(bool firstSpawn) {
    if (!firstSpawn) {
        currentTetromino = nextTetromino;
    }

    if (tetrominoBag.empty()) {
        tetrominoBag = { 0, 1, 2, 3, 4, 5, 6 };

        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(tetrominoBag.begin(), tetrominoBag.end(), rng);
    }

    nextTetromino = tetrominoShapes[tetrominoBag.back()];
    tetrominoBag.pop_back();

    if (!firstSpawn) {
        switch (currentTetromino.type) {
        case 0: // I-piece (4x4)
            piecePosition = { 3.0f, -1.0f };
            break;
        case 3: // O-piece (2x2)
            piecePosition = { 4.0f, 0.0f };
            break;
        default: // JLSTZ (3x3)
            piecePosition = { 4.0f, 0.0f };
            break;
        }

        if (!isValidMove(currentTetromino, piecePosition)) {
            gameOver = true;
            gameOverTime = std::chrono::steady_clock::now();
            if (std::get<BoolValue>(audio)) {
                util::PlaySoundUI("game.player.die");
            }
            return;
        }
    }

    // Initialize next piece preview board
    const int previewOffset = (NEXT_SIZE - nextTetromino.dimension) / 2;
    nextBoard = std::vector(NEXT_SIZE, std::vector<int>(NEXT_SIZE, 0));

    for (int y = 0; y < nextTetromino.dimension; y++) {
        for (int x = 0; x < nextTetromino.dimension; x++) {
            if (nextTetromino.shape[y][x]) {
                int px = x + previewOffset;
                int py = y + previewOffset;
                if (px >= 0 && px < NEXT_SIZE && py >= 0 && py < NEXT_SIZE) {
                    nextBoard[py][px] = nextTetromino.shape[y][x];
                }
            }
        }
    }

    canHold = true;
}

std::vector<Vec2> BlockGame::getKicks(int type, int fromState, int toState) {
    if (type == 0) {
        return I_KICKS[(fromState % 4)];
    } else if (type == 3) {
        return { {0,0} };
    } else {
        return JLSTZ_KICKS[(fromState % 4)];
    }
}


void BlockGame::restartGame() {
    board = std::vector(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));
    score = 0;
    level = 1;
    linesCleared = 0;
    gameOver = false;
    paused = false;
    hasHold = false;
    canHold = true;

    tetrominoBag.clear();
    for (int i = 0; i < 7; ++i)
        tetrominoBag.push_back(i);

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(tetrominoBag.begin(), tetrominoBag.end(), rng);

    currentTetromino = tetrominoShapes[tetrominoBag.back()];
    tetrominoBag.pop_back();

    switch (currentTetromino.type) {
    case 0: piecePosition = {3.0f, -1.0f};
        break;
    case 3: piecePosition = {4.0f, 0.0f};
        break;
    default: piecePosition = {4.0f, 0.0f};
        break;
    }

    spawnTetromino(true);
    lastFall = std::chrono::steady_clock::now();
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
    if (std::get<BoolValue>(audio)) {
        util::PlaySoundUI("random.pop");
    }
}

void BlockGame::rotateTetromino(bool clockwise, bool is180) {
    if (currentTetromino.type == 3) return;

    if (is180) {
        Tetromino rotated = currentTetromino;
        const int size = rotated.dimension;
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                rotated.shape[y][x] = currentTetromino.shape[size - 1 - y][size - 1 - x];
            }
        }
        int newState = (currentTetromino.rotationState + 2) % 4;
        std::vector<Vec2> candidateOffsets = { {0,0}, {1,0}, {-1,0}, {0,1}, {0,-1} };
        for (const auto& off : candidateOffsets) {
            Vec2 newPos = piecePosition + off;
            if (isValidMove(rotated, newPos)) {
                currentTetromino = rotated;
                currentTetromino.rotationState = newState;
                piecePosition = newPos;
                util::PlaySoundUI("random.pop2");
                lastWasRotation = true;
                lastKickOffset = off;
                return;
            }
        }
    } else {
        const int currentState = currentTetromino.rotationState;
        int direction = clockwise ? 1 : -1;
        int newState = (currentState + direction + 4) % 4;
        const auto& offsets = (currentTetromino.type == 0) ? I_OFFSETS : JLSTZ_OFFSETS;
        std::vector<Vec2> kicks;
        for (int i = 0; i < 5; ++i) {
            Vec2 currentOffset = offsets[currentState][i];
            Vec2 newOffset = offsets[newState][i];
            kicks.push_back(currentOffset - newOffset);
        }

        Tetromino rotated = rotateMatrix(currentTetromino, clockwise);

        for (const auto& kick : kicks) {
            Vec2 newPos = piecePosition + kick;
            if (isValidMove(rotated, newPos)) {
                currentTetromino = rotated;
                currentTetromino.rotationState = newState;
                piecePosition = newPos;
                util::PlaySoundUI("random.pop2");
                lastWasRotation = true;
                lastKickOffset = kick;
                return;
            }
        }
    }
}

Tetromino BlockGame::rotateMatrix(const Tetromino& original, bool clockwise) {
    Tetromino rotated = original;
    const int size = rotated.dimension;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (clockwise) {
                rotated.shape[x][size - 1 - y] = original.shape[y][x];
            }
            else {
                rotated.shape[size - 1 - x][y] = original.shape[y][x];
            }
        }
    }
    return rotated;
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
    spawnTetromino(false);
    softDropScore = 0;
}

void BlockGame::mergeTetromino() {
    // tetromino placed sound
    canHold = false;
    if (std::get<BoolValue>(audio)) {
        util::PlaySoundUI("note.bd");
    }

    if (currentTetromino.type == 5 && lastWasRotation) {
        float baseX = piecePosition.x;
        float baseY = piecePosition.y;

        Vec2 topLeft = { baseX, baseY };
        Vec2 topRight = { baseX + 2, baseY };
        Vec2 bottomLeft = { baseX, baseY + 2 };
        Vec2 bottomRight = { baseX + 2, baseY + 2 };

        Vec2 front1, front2, back1, back2;
        switch (currentTetromino.rotationState) {
        case STATE_0:
            front1 = topLeft;
            front2 = topRight;
            back1 = bottomLeft;
            back2 = bottomRight;
            break;
        case STATE_R:
            front1 = topRight;
            front2 = bottomRight;
            back1 = topLeft;
            back2 = bottomLeft;
            break;
        case STATE_2:
            front1 = bottomLeft;
            front2 = bottomRight;
            back1 = topLeft;
            back2 = topRight;
            break;
        case STATE_L:
            front1 = topLeft;
            front2 = bottomLeft;
            back1 = topRight;
            back2 = bottomRight;
            break;
        default:
            front1 = topLeft;
            front2 = topRight;
            back1 = bottomLeft;
            back2 = bottomRight;
            break;
        }

        auto isOccupied = [&](const Vec2& pos) -> bool {
            float x = pos.x;
            float y = pos.y;
            if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
                return true;
            }
            return board[y][x] != 0;
        };

        int frontCount = 0;
        if (isOccupied(front1)) frontCount++;
        if (isOccupied(front2)) frontCount++;
        int backCount = 0;
        if (isOccupied(back1)) backCount++;
        if (isOccupied(back2)) backCount++;

        if ((abs(lastKickOffset.x) == 1 && abs(lastKickOffset.y) == 2) ||
            (abs(lastKickOffset.x) == 2 && abs(lastKickOffset.y) == 1)) {
            lastMoveWasTSpin = true;
            lastMoveWasMiniTSpin = false;
        } else if (frontCount >= 2 && backCount >= 1) {
            lastMoveWasTSpin = true;
            lastMoveWasMiniTSpin = false;
        } else if (frontCount == 1 && backCount == 2) {
            lastMoveWasTSpin = true;
            lastMoveWasMiniTSpin = true;
        } else {
            lastMoveWasTSpin = false;
            lastMoveWasMiniTSpin = false;
        }
    } else {
        lastMoveWasTSpin = false;
        lastMoveWasMiniTSpin = false;
    }

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

    lastWasRotation = false;
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
            if (std::get<BoolValue>(audio)) {
                util::PlaySoundUI("note.pling");
            }
            board.erase(board.begin() + y);
            board.insert(board.begin(), std::vector(BOARD_WIDTH, 0));
            linesClearedThisTurn++;
            y++;
        }
    }

    if (linesClearedThisTurn > 0) {
        if (!lastMoveWasTSpin) {
            score += (linesClearedThisTurn * 100) * level;
        }
    } else {
        if (lastMoveWasTSpin) {
            // todo: figure out scoring for t-spin with no clear (afaik most tetris game will still award points)
        }
    }

    if (lastMoveWasTSpin) {
        int bonus = 0;
        if (lastMoveWasMiniTSpin) {
            // mini t-spin:
            if (linesClearedThisTurn == 0) bonus = 100;
            else if (linesClearedThisTurn == 1) bonus = 200;
            else if (linesClearedThisTurn == 2) bonus = 400;
        } else {
            // the COOL way to t-spin:
            if (linesClearedThisTurn == 0) bonus = 400;
            else if (linesClearedThisTurn == 1) bonus = 800;
            else if (linesClearedThisTurn == 2) bonus = 1200;
            else if (linesClearedThisTurn == 3) bonus = 1600;
        }
        score += bonus * level;
    }

    if (linesClearedThisTurn > 0) {
        linesCleared += linesClearedThisTurn;

        if (linesCleared >= level * 10) {
            if (std::get<BoolValue>(audio)) {
                util::PlaySoundUI("random.levelup");
            }
            level++;
        }
    }

    lastMoveWasTSpin = false;
    lastMoveWasMiniTSpin = false;
}

void BlockGame::onRenderHUDModules(Event& evGeneric) {
    DrawHUDModulesEvent& ev = reinterpret_cast<DrawHUDModulesEvent&>(evGeneric);
    ev.setCancelled(true);
}
