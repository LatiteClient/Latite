#include "pch.h"
#include "BlockGame.h"
#include "client/Latite.h"
#include "client/render/Assets.h"
#include "client/input/Keyboard.h"

BlockGame::BlockGame() : Module("BlockGame", LocalizeString::get("client.module.blockGame.name"),
                                LocalizeString::get("client.module.blockGame.desc"), HUD, nokeybind) {
    listen<RenderOverlayEvent>(static_cast<EventListenerFunc>(&BlockGame::onRenderOverlay));
    listen<KeyUpdateEvent>(static_cast<EventListenerFunc>(&BlockGame::onKey));
    listen<DrawHUDModulesEvent>(static_cast<EventListenerFunc>(&BlockGame::onRenderHUDModules), false, 2);

    addSliderSetting("arrSetting", L"ARR",
                     LocalizeString::get("client.module.blockGame.arrSetting.desc"),
                     arrIntervalSetting, FloatValue(0.f),
                     FloatValue(83.f), FloatValue(1.f));
    addSliderSetting("dasSetting", L"DAS",
                     LocalizeString::get("client.module.blockGame.dasSetting.desc"),
                     dasDelaySetting, FloatValue(17.f),
                     FloatValue(333.f), FloatValue(1.f));
    addSliderSetting("dcdSetting", L"DCD",
                     LocalizeString::get("client.module.blockGame.dcdSetting.desc"),
                     dcdDelaySetting, FloatValue(0.f),
                     FloatValue(333.f), FloatValue(1.f));
    addSliderSetting("sdfSetting", L"SDF",
                     LocalizeString::get("client.module.blockGame.sdfSetting.desc"),
                     sdfSetting, FloatValue(5.f), FloatValue(41.f), FloatValue(1.f));

    addSetting("audioSetting", LocalizeString::get("client.module.blockGame.audioSetting.name"),
               LocalizeString::get("client.module.blockGame.audioSetting.desc"), audio);
    addSetting("drawGridSetting", LocalizeString::get("client.module.blockGame.drawGridSetting.name"),
               LocalizeString::get("client.module.blockGame.drawGridSetting.desc"), drawGrid);
    addSetting("drawGhostPiece", LocalizeString::get("client.module.blockGame.drawGhostPiece.name"),
        LocalizeString::get("client.module.blockGame.drawGhostPiece.desc"), drawGhostPiece);
    addSetting("backgroundEnabledSetting", LocalizeString::get("client.module.blockGame.backgroundEnabledSetting.name"),
               LocalizeString::get("client.module.blockGame.backgroundEnabledSetting.desc"), backgroundEnabled);
    addSetting("backgroundColorSetting", LocalizeString::get("client.module.blockGame.backgroundColorSetting.name"),
               LocalizeString::get("client.module.blockGame.backgroundColorSetting.desc"), backgroundColor,
               "backgroundEnabledSetting"_istrue);
    addSetting("moveLeftSetting", LocalizeString::get("client.module.blockGame.moveLeftSetting.name"),
               LocalizeString::get("client.module.blockGame.moveLeftSetting.desc"), leftKey);
    addSetting("moveRightSetting", LocalizeString::get("client.module.blockGame.moveRightSetting.name"),
               LocalizeString::get("client.module.blockGame.moveRightSetting.desc"), rightKey);
    addSetting("hardDropSetting", LocalizeString::get("client.module.blockGame.hardDropSetting.name"),
               LocalizeString::get("client.module.blockGame.hardDropSetting.desc"), hardDropKey);
    addSetting("softDropSetting", LocalizeString::get("client.module.blockGame.softDropSetting.name"),
               LocalizeString::get("client.module.blockGame.softDropSetting.desc"), softDropKey);
    addSetting("rotateCWSetting", LocalizeString::get("client.module.blockGame.rotateSetting.name"),
               LocalizeString::get("client.module.blockGame.rotateSetting.desc"), rotateKey);
    addSetting("rotateCCWSetting", LocalizeString::get("client.module.blockGame.rotateCCWSetting.name"),
               LocalizeString::get("client.module.blockGame.rotateCCWSetting.desc"), rotateCCWKey);
    addSetting("rotate180Setting", LocalizeString::get("client.module.blockGame.rotate180Setting.name"),
               LocalizeString::get("client.module.blockGame.rotate180Setting.desc"), rotate180Key);
    addSetting("holdSetting", LocalizeString::get("client.module.blockGame.holdSetting.name"),
               LocalizeString::get("client.module.blockGame.holdSetting.desc"), holdKey);
    addSetting("pauseSetting", LocalizeString::get("client.module.blockGame.pauseSetting.name"),
               LocalizeString::get("client.module.blockGame.pauseSetting.desc"), pauseKey);
    addSetting("restartSetting", LocalizeString::get("client.module.blockGame.restartSetting.name"),
               LocalizeString::get("client.module.blockGame.restartSetting.desc"), restartKey);

    createTetrominoShapes();
    restartGame();
}

void BlockGame::onEnable() {
    restartGame();
    paused = false;
    leftKeyHeld = false;
    rightKeyHeld = false;
    softDropKeyHeld = false;
    dasStateLeft = DasState::IDLE;
    dasStateRight = DasState::IDLE;
}

void BlockGame::onDisable() {
    paused = true;
}


void BlockGame::updateGameLogic(std::chrono::steady_clock::time_point now) {
    if (gameOver || paused) return;

    // subject to change, i dont think i got this perfect yet
    std::chrono::milliseconds baseGravityDelay;
    if (level <= 1) baseGravityDelay = std::chrono::milliseconds(800);
    else if (level == 2) baseGravityDelay = std::chrono::milliseconds(717);
    else if (level == 3) baseGravityDelay = std::chrono::milliseconds(633);
    else if (level == 4) baseGravityDelay = std::chrono::milliseconds(550);
    else if (level == 5) baseGravityDelay = std::chrono::milliseconds(467);
    else if (level == 6) baseGravityDelay = std::chrono::milliseconds(383);
    else if (level == 7) baseGravityDelay = std::chrono::milliseconds(300);
    else if (level == 8) baseGravityDelay = std::chrono::milliseconds(217);
    else if (level == 9) baseGravityDelay = std::chrono::milliseconds(120);
    else if (level <= 12) baseGravityDelay = std::chrono::milliseconds(84);
    else if (level <= 15) baseGravityDelay = std::chrono::milliseconds(67);
    else if (level <= 18) baseGravityDelay = std::chrono::milliseconds(43);
    else if (level <= 28) baseGravityDelay = std::chrono::milliseconds(20);
    else baseGravityDelay = std::chrono::milliseconds(7);

    std::chrono::milliseconds currentGravityInterval = baseGravityDelay;
    float sdfValue = std::get<FloatValue>(sdfSetting);

    if (softDropKeyHeld) {
        if (sdfValue >= 41.0f) {
            // instant drop
            bool moved = false;
            while (testMove(0, 1)) {
                applyMove(0, 1);
                score += 1;
                moved = true;
            }

            if (moved) {
                lastFall = now;
            } else {
                if (!isLocking) {
                    isLocking = true;
                    lockStartTime = now;
                    lockResets = 0;
                }
            }
            return;
        } else {
            // regular scuffed sdf
            auto acceleratedMs = static_cast<int64_t>(baseGravityDelay.count() / sdfValue);
            currentGravityInterval = std::chrono::milliseconds(std::max(1LL, acceleratedMs));
        }
    }

    if (now - lastFall >= currentGravityInterval) {
        if (testMove(0, 1)) {
            applyMove(0, 1);
            if (softDropKeyHeld) score += 1;
        } else {
            if (!isLocking) {
                isLocking = true;
                lockStartTime = now;
                lockResets = 0;
            }
        }
        lastFall = now;
    }

    if (isLocking) {
        if (testMove(0, 1)) {
            isLocking = false;
        } else {
            std::chrono::milliseconds lockDelayDuration = std::chrono::milliseconds(500);
            if (now - lockStartTime >= lockDelayDuration) {
                mergeTetromino();
                int lines = clearLinesAndScore(lastMoveWasTSpin, lastMoveWasMiniTSpin);
                if (!gameOver) {
                    spawnTetromino(false);
                }
            }
        }
    }
}

void BlockGame::handleInput(std::chrono::steady_clock::time_point now) {
    if (gameOver || paused) return;

    int vkLeft = std::get<KeyValue>(leftKey);
    int vkRight = std::get<KeyValue>(rightKey);
    int vkSoftDrop = std::get<KeyValue>(softDropKey);

    std::chrono::milliseconds dasDelay { static_cast<int>(std::get<FloatValue>(dasDelaySetting)) };
    std::chrono::milliseconds arrInterval { static_cast<int>(std::get<FloatValue>(arrIntervalSetting)) };
    std::chrono::milliseconds dcdDelay { static_cast<int>(std::get<FloatValue>(dcdDelaySetting)) };

    leftKeyHeld = isKeyDown(vkLeft);
    rightKeyHeld = isKeyDown(vkRight);
    softDropKeyHeld = isKeyDown(vkSoftDrop);

    if (leftKeyHeld) {
        if (dasStateLeft == DasState::IDLE && now >= dcdEndTimeLeft) {
            tryMoveHorizontal(-1);
            dasStateLeft = DasState::CHARGING;
            dasStartTimeLeft = now;
            if (dasStateRight != DasState::IDLE) {
                dcdEndTimeRight = now + dcdDelay;
                dasStateRight = DasState::IDLE;
            }
        } else if (dasStateLeft == DasState::CHARGING && now - dasStartTimeLeft >= dasDelay) {
            dasStateLeft = DasState::REPEATING;
            arrTimeLeft = now;
            tryMoveHorizontal(-1);
        } else if (dasStateLeft == DasState::REPEATING && now >= arrTimeLeft) {
            arrTimeLeft = now + arrInterval;
            tryMoveHorizontal(-1);
        }
    } else {
        if (dasStateLeft != DasState::IDLE) {
            dasStateLeft = DasState::IDLE;
            if (!rightKeyHeld) dcdEndTimeRight = now;
        }
    }

    if (rightKeyHeld) {
        if (dasStateRight == DasState::IDLE && now >= dcdEndTimeRight) {
            tryMoveHorizontal(1);
            dasStateRight = DasState::CHARGING;
            dasStartTimeRight = now;
            if (dasStateLeft != DasState::IDLE) {
                dcdEndTimeLeft = now + dcdDelay;
                dasStateLeft = DasState::IDLE;
            }
        } else if (dasStateRight == DasState::CHARGING && now - dasStartTimeRight >= dasDelay) {
            dasStateRight = DasState::REPEATING;
            arrTimeRight = now;
            tryMoveHorizontal(1);
        } else if (dasStateRight == DasState::REPEATING && now >= arrTimeRight) {
            arrTimeRight = now + arrInterval;
            tryMoveHorizontal(1);
        }
    } else {
        if (dasStateRight != DasState::IDLE) {
            dasStateRight = DasState::IDLE;
            if (!leftKeyHeld) dcdEndTimeLeft = now;
        }
    }

    if (!leftKeyHeld && now >= dcdEndTimeLeft) dcdEndTimeLeft = now;
    if (!rightKeyHeld && now >= dcdEndTimeRight) dcdEndTimeRight = now;
}

void BlockGame::onKey(Event& evG) {
    KeyUpdateEvent& ev = reinterpret_cast<KeyUpdateEvent&>(evG);

    if (ev.getKey() == std::get<KeyValue>(pauseKey) && ev.isDown()) {
        paused = !paused;
        // reset timing references to avoid large jumps when unpausing
        if (!paused) {
            lastFall = std::chrono::steady_clock::now();
            if (isLocking) lockStartTime = std::chrono::steady_clock::now();
        }
        playSound("random.click");
        return;
    }

    if (gameOver && ev.getKey() == std::get<KeyValue>(restartKey) && ev.isDown()) {
        playSound("note.snare");
        restartGame();
        return;
    }

    if (gameOver || paused || !ev.isDown()) {
        if (!ev.isDown() && ev.getKey() == std::get<KeyValue>(softDropKey)) {
            softDropKeyHeld = false;
        }
        return;
    }

    KeyValue key = ev.getKey();

    if (key == std::get<KeyValue>(rotateKey)) {
        if (rotateTetromino(true)) {
            playSound("random.pop2");
        }
    } else if (key == std::get<KeyValue>(rotateCCWKey)) {
        if (rotateTetromino(false)) {
            playSound("random.pop2");
        }
    } else if (key == std::get<KeyValue>(rotate180Key)) {
        if (handle180Rotation()) {
            playSound("random.pop2");
        }
    } else if (key == std::get<KeyValue>(hardDropKey)) {
        hardDrop();
    } else if (key == std::get<KeyValue>(holdKey)) {
        handleHold();
    } else if (key == std::get<KeyValue>(restartKey)) {
        playSound("note.snare");
        restartGame();
    }
}

void BlockGame::onRenderOverlay(Event& evG) {
    RenderOverlayEvent& ev = reinterpret_cast<RenderOverlayEvent&>(evG);

    if (!SDK::ClientInstance::get() || !SDK::ClientInstance::get()->getLocalPlayer() || !SDK::ClientInstance::get()->
        minecraftGame->isCursorGrabbed()) {
        if (!gameOver && !paused) {
            paused = true;
        }
        return;
    }

    auto now = std::chrono::steady_clock::now();
    handleInput(now);
    updateGameLogic(now);

    D2DUtil dc;
    auto [screenWidth, screenHeight] = Latite::getRenderer().getScreenSize();

    // I HATE SIZE CALCULATIONS!!! I HATE THIS!!! I SPENT HOURS ON THIS!!!!
    const float baseBlockSize = screenHeight * 0.035f;
    const float blockSize = std::max(10.0f, baseBlockSize);
    const float boardWidthPixels = BOARD_WIDTH * blockSize;
    const float boardHeightPixels = BOARD_HEIGHT * blockSize;
    const float boardLeft = (screenWidth - boardWidthPixels) * 0.75f;
    const float boardTop = (screenHeight - boardHeightPixels) / 2.0f;

    const float nextPreviewX = boardLeft + boardWidthPixels + blockSize;
    const float nextPreviewY = boardTop;
    const float holdPreviewX = boardLeft - (HOLD_SIZE + 1) * blockSize;
    const float holdPreviewY = boardTop;

    StoredColor bgColor = std::get<ColorValue>(backgroundColor).getMainColor();

    // draw pause/game over text
    if (paused) {
        dc.drawText({
                        screenWidth * 0.5f - 150, screenHeight * 0.5f - 50, screenWidth * 0.5f + 150,
                        screenHeight * 0.5f + 50
                    },
                    LocalizeString::get("client.module.blockGame.pausedText.name"),
                    d2d::Colors::YELLOW, Renderer::FontSelection::PrimaryRegular, 48.0f,
                    DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        return;
    }

    if (gameOver) {
        long long elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - gameOverTime).count();
        int remaining = 5 - static_cast<int>(elapsed);

        std::wstring statusText = LocalizeString::get("client.module.blockGame.gameOverText.name");
        std::wstring countdown = util::FormatWString(
            LocalizeString::get("client.module.blockGame.restartCountdown.name"),
            { std::to_wstring(std::max(0, remaining)) });

        if (remaining <= 0) {
            playSound("note.snare");
            restartGame();
            return;
        }

        dc.drawText(
            { screenWidth * 0.5f - 150, screenHeight * 0.5f - 100, screenWidth * 0.5f + 150, screenHeight * 0.5f },
            statusText, d2d::Colors::RED, Renderer::FontSelection::PrimaryRegular, 48.0f,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        dc.drawText({ screenWidth * 0.5f - 150, screenHeight * 0.5f, screenWidth * 0.5f + 150, screenHeight * 0.5f + 50 },
                    countdown, d2d::Colors::YELLOW, Renderer::FontSelection::PrimaryRegular, 24.0f,
                    DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        // display stats below countdown
        std::wstring stats = util::FormatWString(LocalizeString::get("client.module.blockGame.scoreText.name"),
                                                 { std::to_wstring(score) })
            + L"\n" + util::FormatWString(LocalizeString::get("client.module.blockGame.levelText.name"),
                                          { std::to_wstring(level) })
            + L"\n" + util::FormatWString(LocalizeString::get("client.module.blockGame.linesClearedText.name"),
                                          { std::to_wstring(linesCleared) });
        dc.drawText({
                        screenWidth * 0.5f - 150, screenHeight * 0.5f + 60, screenWidth * 0.5f + 150,
                        screenHeight * 0.5f + 160
                    },
                    stats, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular, 20.0f,
                    DWRITE_TEXT_ALIGNMENT_CENTER);

        return;
    }


    // draw board
    const float borderThickness = 2.0f;
    // draw border slightly outset
    dc.drawRectangle(
        {
            boardLeft - borderThickness, boardTop - borderThickness,
            boardLeft + boardWidthPixels + borderThickness, boardTop + boardHeightPixels + borderThickness
        },
        d2d::Colors::WHITE, borderThickness);

    // draw board background
    if (bgColor.a > 0.0f && std::get<BoolValue>(backgroundEnabled)) {
        d2d::Color backgroundColor = {bgColor.r, bgColor.b,
            bgColor.g, bgColor.a};
        dc.fillRectangle(
            { boardLeft, boardTop, boardLeft + boardWidthPixels, boardTop + boardHeightPixels },
            backgroundColor
        );
    }

    // draw grid lines
    if (std::get<BoolValue>(drawGrid)) {
        const float gridAlpha = 0.15f;
        const auto gridColor = d2d::Color::Hex("FFFFFF", gridAlpha);
        const float lineThickness = 1.0f;
        for (int x = 1; x < BOARD_WIDTH; ++x) {
            float xPos = boardLeft + x * blockSize;
            dc.fillRectangle({
                                 xPos - lineThickness / 2.0f, boardTop, xPos + lineThickness / 2.0f,
                                 boardTop + boardHeightPixels
                             }, gridColor);
        }
        for (int y = 1; y < BOARD_HEIGHT; ++y) {
            float yPos = boardTop + y * blockSize;
            dc.fillRectangle({
                                 boardLeft, yPos - lineThickness / 2.0f, boardLeft + boardWidthPixels,
                                 yPos + lineThickness / 2.0f
                             }, gridColor);
        }
    }

    // draw static blocks on the board (only visible area)
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            int boardY = y + BOARD_BUFFER;
            if (board[boardY][x] != 0) {
                // 0 represents Empty
                dc.fillRectangle(
                    {
                        boardLeft + x * blockSize, boardTop + y * blockSize,
                        boardLeft + (x + 1) * blockSize, boardTop + (y + 1) * blockSize
                    },
                    tetrominoShapes[board[boardY][x] - 1].color
                );
            }
        }
    }

    // draw ghost piece
    if (std::get<BoolValue>(drawGhostPiece)) {
        Tetromino ghostPiece = currentTetromino;
        Vec2 ghostPosition = piecePosition;
        while (isValidPosition(ghostPiece, { ghostPosition.x, ghostPosition.y + 1 })) {
            ghostPosition.y++;
        }
        float ghostAlpha = 0.3f;
        for (int y = 0; y < ghostPiece.dimension; ++y) {
            for (int x = 0; x < ghostPiece.dimension; ++x) {
                if (ghostPiece.shape[y][x] != 0) {
                    float drawX = boardLeft + (ghostPosition.x + x) * blockSize;
                    float drawY = boardTop + (ghostPosition.y + y - BOARD_BUFFER) * blockSize;
                    if (ghostPosition.y + y >= BOARD_BUFFER) {
                        dc.fillRectangle(
                            {drawX, drawY, drawX + blockSize, drawY + blockSize},
                            d2d::Color::Hex(ghostPiece.color.getHex(), ghostAlpha)
                        );
                    }
                }
            }
        }
    }

    // draw current piece
    for (int y = 0; y < currentTetromino.dimension; ++y) {
        for (int x = 0; x < currentTetromino.dimension; ++x) {
            if (currentTetromino.shape[y][x] != 0) {
                float drawX = boardLeft + (piecePosition.x + x) * blockSize;
                float drawY = boardTop + (piecePosition.y + y - BOARD_BUFFER) * blockSize;
                // Only draw blocks within the visible board area
                if (piecePosition.y + y >= BOARD_BUFFER) {
                    dc.fillRectangle(
                        { drawX, drawY, drawX + blockSize, drawY + blockSize },
                        currentTetromino.color
                    );
                }
            }
        }
    }

    // draw hold piece area background
    if (bgColor.a > 0.0f && std::get<BoolValue>(backgroundEnabled)) {
        d2d::Color backgroundColor = { bgColor.r, bgColor.b, bgColor.g, bgColor.a };
        dc.fillRectangle(
            {
                holdPreviewX - 2, holdPreviewY + 35, holdPreviewX + HOLD_SIZE * blockSize + 2,
                holdPreviewY + 45 + HOLD_SIZE * blockSize
            },
            backgroundColor
        );
    }

    // draw hold piece
    dc.drawText({ holdPreviewX, holdPreviewY, holdPreviewX + HOLD_SIZE * blockSize, holdPreviewY + 30 },
                LocalizeString::get("client.module.blockGame.holdPieceText.name"),
                canHold ? d2d::Colors::WHITE : d2d::Color::Hex("808080"),
                Renderer::FontSelection::PrimaryRegular);

    if (hasHold) {
        // Center the hold piece visually in its box
        float holdOffsetX = (HOLD_SIZE - holdTetromino.dimension) / 2.0f * blockSize;
        float holdOffsetY = (HOLD_SIZE - holdTetromino.dimension) / 2.0f * blockSize;
        holdTetromino.rotationState = RotationState::STATE_0;
        for (int y = 0; y < holdTetromino.dimension; ++y) {
            for (int x = 0; x < holdTetromino.dimension; ++x) {
                if (holdTetromino.shape[y][x] != 0) {
                    dc.fillRectangle(
                        {
                            holdPreviewX + holdOffsetX + x * blockSize, holdPreviewY + 40 + holdOffsetY + y * blockSize,
                            holdPreviewX + holdOffsetX + (x + 1) * blockSize,
                            holdPreviewY + 40 + holdOffsetY + (y + 1) * blockSize
                        },
                        holdTetromino.color
                    );
                }
            }
        }
    }

    // draw hold piece area
    dc.drawRectangle({
                         holdPreviewX - 2, holdPreviewY + 35, holdPreviewX + HOLD_SIZE * blockSize + 2,
                         holdPreviewY + 45 + HOLD_SIZE * blockSize
                     }, d2d::Colors::WHITE, 1.0f);

    // draw next piece area background
    if (bgColor.a > 0.0f && std::get<BoolValue>(backgroundEnabled)) {
        d2d::Color backgroundColor = { bgColor.r, bgColor.b, bgColor.g, bgColor.a };
        dc.fillRectangle(
            {
                nextPreviewX - 2, nextPreviewY + 35, nextPreviewX + NEXT_SIZE * blockSize + 2,
                nextPreviewY + 45 + NEXT_SIZE * blockSize
            },
            backgroundColor
        );
    }

    // draw next piece
    dc.drawText({ nextPreviewX, nextPreviewY, nextPreviewX + NEXT_SIZE * blockSize, nextPreviewY + 30 },
                LocalizeString::get("client.module.blockGame.nextPiecePreviewText.name"), d2d::Colors::WHITE,
                Renderer::FontSelection::PrimaryRegular);

    float nextOffsetX = (NEXT_SIZE - nextTetromino.dimension) / 2.0f * blockSize;
    float nextOffsetY = (NEXT_SIZE - nextTetromino.dimension) / 2.0f * blockSize;
    for (int y = 0; y < nextTetromino.dimension; ++y) {
        for (int x = 0; x < nextTetromino.dimension; ++x) {
            if (nextTetromino.shape[y][x] != 0) {
                dc.fillRectangle(
                    {
                        nextPreviewX + nextOffsetX + x * blockSize, nextPreviewY + 40 + nextOffsetY + y * blockSize,
                        nextPreviewX + nextOffsetX + (x + 1) * blockSize,
                        nextPreviewY + 40 + nextOffsetY + (y + 1) * blockSize
                    },
                    nextTetromino.color
                );
            }
        }
    }

    // draw next piece area
    dc.drawRectangle({
                         nextPreviewX - 2, nextPreviewY + 35, nextPreviewX + NEXT_SIZE * blockSize + 2,
                         nextPreviewY + 45 + NEXT_SIZE * blockSize
                     }, d2d::Colors::WHITE, 1.0f);


    // draw stats
    float mainStatsX = nextPreviewX;
    float mainStatsY = nextPreviewY + (NEXT_SIZE + 1.5f) * blockSize;

    float otherStatsX = mainStatsX + 100;
    float otherStatsY = mainStatsY + 100;

    float textHeight = 20.0f;
    float lineSpacing = 5.0f;

    std::wstring scoreStr = util::FormatWString(LocalizeString::get("client.module.blockGame.scoreText.name"),
                                                { std::to_wstring(score) });
    std::wstring levelStr = util::FormatWString(LocalizeString::get("client.module.blockGame.levelText.name"),
                                                { std::to_wstring(level) });
    std::wstring linesStr = util::FormatWString(LocalizeString::get("client.module.blockGame.linesClearedText.name"),
                                                { std::to_wstring(linesCleared) });

    dc.drawText({ nextPreviewX, mainStatsY, nextPreviewX + 200, mainStatsX + textHeight }, scoreStr, d2d::Colors::WHITE,
                Renderer::FontSelection::PrimaryRegular, textHeight);
    dc.drawText({ nextPreviewX, mainStatsY + textHeight + 5, nextPreviewX + 200, mainStatsX + textHeight * 2 + 5 },
                levelStr, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular, textHeight);
    dc.drawText({ nextPreviewX, mainStatsY + textHeight * 2 + 10, nextPreviewX + 200, mainStatsX + textHeight * 3 + 10 },
                linesStr, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular, textHeight);

    if (!tSpinText.empty() && now - tSpinDisplayTime < tSpinDisplayDuration) {
        float tSpinTextSize = textHeight * 1.1f;
        auto tSpinTextColor = d2d::Color::RGB(160, 0, 240);

        if (now - tSpinDisplayTime < displayFlashDuration) {
            tSpinTextSize *= 1.2f;
            tSpinTextColor = d2d::Color::RGB(200, 115, 242);
        }

        dc.drawText({ nextPreviewX, otherStatsY + textHeight * 2 + 5, nextPreviewX + 200, otherStatsX + tSpinTextSize },
            tSpinText, tSpinTextColor, Renderer::FontSelection::PrimaryRegular, tSpinTextSize);
        otherStatsY += tSpinTextSize + lineSpacing;
    }

    if (!lineClearText.empty() && now - lineClearDisplayTime < lineClearDisplayDuration) {
        float lineClearTextSize = textHeight * 1.1f;
        auto lineClearTextColor = d2d::Colors::WHITE;

        if (now - lineClearDisplayTime < displayFlashDuration) {
            lineClearTextSize *= 1.1f;
            if (lineClearText == L"TETRIS") {
                lineClearTextSize *= 1.2f;
                lineClearTextColor = d2d::Color::Hex("D53D3D");
            } else {
                lineClearTextSize *= 1.1f;
                lineClearTextColor = d2d::Color::Hex("FB6464");
            }
        }

        dc.drawText({ nextPreviewX, otherStatsY + textHeight * 2 + 10, nextPreviewX + 200, otherStatsX + lineClearTextSize },
            lineClearText, lineClearTextColor, Renderer::FontSelection::PrimaryRegular, lineClearTextSize);
        otherStatsY += lineClearTextSize + lineSpacing;
    }

    if (b2bCount > 1) {
        std::wstring b2bText = L"B2B " + std::to_wstring(b2bCount) + L"x";
        float b2bSize = textHeight * 1.1f;
        auto b2bColor = d2d::Color::Hex("00FFFF");

        if (now - b2bUpdateTime < displayFlashDuration) {
            b2bSize *= 1.1f;
            b2bColor = d2d::Color::Hex("ADD8E6");
        }

        dc.drawText({ nextPreviewX, otherStatsY + textHeight * 2 + 15, nextPreviewX + 200, otherStatsX + b2bSize },
            b2bText, b2bColor, Renderer::FontSelection::PrimaryRegular, b2bSize);
        otherStatsY += b2bSize + lineSpacing;
    }

    if (comboCount > 1) {
        std::wstring comboText = L"Combo " + std::to_wstring(comboCount) + L"x";
        float comboSize = textHeight * 1.1f;
        auto comboColor = d2d::Color::Hex("FFA500");

        if (now - comboUpdateTime < displayFlashDuration) {
            comboSize *= 1.1f;
            comboColor = d2d::Colors::YELLOW;
        }

        dc.drawText({ nextPreviewX, otherStatsY + textHeight * 2 + 20, nextPreviewX + 200, otherStatsX + comboSize },
                    comboText, comboColor, Renderer::FontSelection::PrimaryRegular, comboSize);
        otherStatsY += comboSize + lineSpacing;
    }
}

void BlockGame::handleHold() {
    if (!canHold) return;

    playSound("note.bass");

    if (!hasHold) {
        holdTetromino = tetrominoShapes[currentTetromino.type];
        holdTetromino.rotationState = RotationState::STATE_0;

        hasHold = true;
        spawnTetromino(false);
    } else {
        Tetromino tempHold = holdTetromino;

        holdTetromino = tetrominoShapes[currentTetromino.type];
        holdTetromino.rotationState = RotationState::STATE_0;

        currentTetromino = tempHold;
        holdTetromino.rotationState = RotationState::STATE_0;

        piecePosition = {static_cast<float>(BOARD_WIDTH / 2 - currentTetromino.dimension / 2), 0.0f};
        if (currentTetromino.type == TetrominoType::I_PIECE) piecePosition = {3.0f, 0.0f};
        else if (currentTetromino.type == TetrominoType::O_PIECE) piecePosition = {4.0f, 0.0f};

        while (!isValidPosition(currentTetromino, piecePosition) && piecePosition.y < BOARD_BUFFER) {
            piecePosition.y++;
        }


        if (!isValidPosition(currentTetromino, piecePosition)) {
            gameOver = true;
            gameOverTime = std::chrono::steady_clock::now();
            playSound("game.player.die");
        }
    }
    canHold = false;
    isLocking = false;
    lastActionWasRotation = false;
}

void BlockGame::createTetrominoShapes() {
    // Shapes defined with their rotation=0 state. Values 1-7 correspond to types.
    // Using 4x4 grid internally simplifies rotation logic.
    // I-shape (Type 1, Index 0) - Cyan - Uses 4x4
    tetrominoShapes[0] = {
        std::to_array<std::array<int, 4>>({{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}), d2d::Color::RGB(0, 240, 240), 4, TetrominoType::I_PIECE
    };
    // J-shape (Type 2, Index 1) - Blue - Uses 3x3 (padded to 4x4)
    tetrominoShapes[1] = {
        std::to_array<std::array<int, 4>>({{2, 0, 0, 0}, {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}), d2d::Color::RGB(0, 0, 240), 3, TetrominoType::J_PIECE
    };
    // L-shape (Type 3, Index 2) - Orange - Uses 3x3
    tetrominoShapes[2] = {
        std::to_array<std::array<int, 4>>({{0, 0, 3, 0}, {3, 3, 3, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}), d2d::Color::RGB(240, 160, 0), 3, TetrominoType::L_PIECE
    };
    // O-shape (Type 4, Index 3) - Yellow - Uses 2x2
    tetrominoShapes[3] = {
        std::to_array<std::array<int, 4>>({{4, 4, 0, 0}, {4, 4, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}), d2d::Color::RGB(240, 240, 0), 2, TetrominoType::O_PIECE
    };
    // S-shape (Type 5, Index 4) - Green - Uses 3x3
    tetrominoShapes[4] = {
        std::to_array<std::array<int, 4>>({{0, 5, 5, 0}, {5, 5, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}), d2d::Color::RGB(0, 240, 0), 3, TetrominoType::S_PIECE
    };
    // T-shape (Type 6, Index 5) - Purple - Uses 3x3
    tetrominoShapes[5] = {
        std::to_array<std::array<int, 4>>({{0, 6, 0, 0}, {6, 6, 6, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}), d2d::Color::RGB(160, 0, 240), 3, TetrominoType::T_PIECE
    };
    // Z-shape (Type 7, Index 6) - Red - Uses 3x3
    tetrominoShapes[6] = {
        std::to_array<std::array<int, 4>>({{7, 7, 0, 0}, {0, 7, 7, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}), d2d::Color::RGB(240, 0, 0), 3, TetrominoType::Z_PIECE
    };

    // hopefully resize fixes
    if (tetrominoShapes.empty()) {
        Logger::Fatal("Failed to initialize tetromino shapes");
    }
}


void BlockGame::spawnTetromino(bool firstSpawn) {
    if (tetrominoBag.empty()) {
        tetrominoBag = { 0, 1, 2, 3, 4, 5, 6 };
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(tetrominoBag.begin(), tetrominoBag.end(), rng);
    }

    if (!firstSpawn) {
        currentTetromino = nextTetromino;
    }

    currentTetromino.rotationState = RotationState::STATE_0;

    int nextTypeIndex = tetrominoBag.back();
    tetrominoBag.pop_back();
    nextTetromino = tetrominoShapes[nextTypeIndex];
    nextTetromino.rotationState = RotationState::STATE_0;

    if (firstSpawn) {
        int firstTypeIndex = tetrominoBag.back();
        tetrominoBag.pop_back();
        currentTetromino = tetrominoShapes[firstTypeIndex];
        currentTetromino.rotationState = RotationState::STATE_0;
    }

    piecePosition = { static_cast<float>(BOARD_WIDTH / 2 - currentTetromino.dimension / 2), 0.0f };
    if (currentTetromino.type == TetrominoType::I_PIECE) piecePosition = { 3.0f, 0.0f };
    else if (currentTetromino.type == TetrominoType::O_PIECE) piecePosition = { 4.0f, 0.0f };

    if (currentTetromino.type == TetrominoType::I_PIECE) piecePosition.y = 0;
    else piecePosition.y = 1;


    if (!isValidPosition(currentTetromino, piecePosition)) {
        if (isValidPosition(currentTetromino, { piecePosition.x, piecePosition.y - 1 })) {
            piecePosition.y--;
        } else {
            gameOver = true;
            gameOverTime = std::chrono::steady_clock::now();
            playSound("game.player.die");
            return;
        }
    }

    canHold = true;
    isLocking = false;
    lastActionWasRotation = false;
    lastMoveWasTSpin = false;
    lastMoveWasMiniTSpin = false;

    if (!leftKeyHeld) dasStateLeft = DasState::IDLE;
    if (!rightKeyHeld) dasStateRight = DasState::IDLE;

    lastFall = std::chrono::steady_clock::now();
}

void BlockGame::restartGame() {
    board = {}; // Initialize with buffer
    score = 0;
    level = 1;
    linesCleared = 0;
    b2bCount = 0;
    comboCount = 0;
    gameOver = false;
    paused = false;
    hasHold = false;
    canHold = true;
    lastMoveWasTSpin = false;
    lastMoveWasMiniTSpin = false;
    isLocking = false;

    tetrominoBag.clear();

    leftKeyHeld = false;
    rightKeyHeld = false;
    softDropKeyHeld = false;
    dasStateLeft = DasState::IDLE;
    dasStateRight = DasState::IDLE;
    dcdEndTimeLeft = std::chrono::steady_clock::now();
    dcdEndTimeRight = std::chrono::steady_clock::now();

    spawnTetromino(true);
    lastFall = std::chrono::steady_clock::now();
}

bool BlockGame::isValidPosition(const Tetromino& tetro, Vec2 pos) {
    for (int y = 0; y < tetro.dimension; ++y) {
        for (int x = 0; x < tetro.dimension; ++x) {
            if (tetro.shape[y][x] != 0) {
                int boardX = static_cast<int>(pos.x + x);
                int boardY = static_cast<int>(pos.y + y);

                if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= TOTAL_BOARD_HEIGHT) {
                    return false;
                }

                if (boardY >= 0 && board[boardY][boardX] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool BlockGame::testMove(int dx, int dy) {
    return isValidPosition(currentTetromino, piecePosition + Vec2{static_cast<float>(dx), static_cast<float>(dy)});
}

void BlockGame::applyMove(int dx, int dy) {
    piecePosition.x += dx;
    piecePosition.y += dy;
    lastActionWasRotation = false;

    if (dx != 0 || dy < 0) {
        if (isLocking && lockResets < MAX_LOCK_RESETS) {
            lockStartTime = std::chrono::steady_clock::now();
            lockResets++;
        }
    }
}

void BlockGame::tryMoveHorizontal(int dx) {
    if (testMove(dx, 0)) {
        applyMove(dx, 0);
        playSound("random.pop");
    }
}

bool BlockGame::rotateTetromino(bool clockwise) {
    if (currentTetromino.type == TetrominoType::O_PIECE) return false;

    int currentState = currentTetromino.rotationState;
    int direction = clockwise ? 1 : -1;
    int newState = (currentState + direction + 4) % 4;

    Tetromino rotated = getRotatedTetromino(currentTetromino, clockwise);
    rotated.rotationState = static_cast<RotationState>(newState);

    const KickTable* kicks = nullptr;
    if (currentTetromino.type == TetrominoType::I_PIECE) {
        kicks = &srsKickDataI;
    } else {
        kicks = &srsKickDataJLSTZ;
    }

    int kickIndex = -1;
    // Map (currentState, newState) to the correct index (0-7) in the KickTable
    if (currentState == 0 && newState == 1) kickIndex = 0; // 0->R
    else if (currentState == 1 && newState == 0) kickIndex = 1; // R->0
    else if (currentState == 1 && newState == 2) kickIndex = 2; // R->2
    else if (currentState == 2 && newState == 1) kickIndex = 3; // 2->R
    else if (currentState == 2 && newState == 3) kickIndex = 4; // 2->L
    else if (currentState == 3 && newState == 2) kickIndex = 5; // L->2
    else if (currentState == 3 && newState == 0) kickIndex = 6; // L->0
    else if (currentState == 0 && newState == 3) kickIndex = 7; // 0->L

    if (!kicks || kickIndex == -1) {
        Logger::Fatal("Invalid kick state calculated in rotateTetromino");
        return false;
    }

    for (const auto& kick : (*kicks)[kickIndex]) {
        // IMPORTANT: SRS kick data is (X, Y) where Y is UP. Our board Y is DOWN.
        Vec2 testPos = piecePosition + Vec2{static_cast<float>(kick.x), static_cast<float>(-kick.y)}; // Invert Y kick

        if (isValidPosition(rotated, testPos)) {
            piecePosition = testPos;
            currentTetromino = rotated;
            lastActionWasRotation = true;
            lastKickOffset = kick;

            if (isLocking && lockResets < MAX_LOCK_RESETS) {
                lockStartTime = std::chrono::steady_clock::now();
                lockResets++;
            } else if (!isLocking && !testMove(0, 1)) {
                isLocking = true;
                lockStartTime = std::chrono::steady_clock::now();
                lockResets = 0;
            }

            return true;
        }
    }

    lastActionWasRotation = false;
    lastKickOffset = { 0, 0 };
    return false;
}

BlockGame::Tetromino BlockGame::getRotatedTetromino(const Tetromino& original, bool clockwise) {
    Tetromino rotated = original;
    const int size = rotated.dimension;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (clockwise) {
                rotated.shape[x][size - 1 - y] = original.shape[y][x];
            } else {
                rotated.shape[size - 1 - x][y] = original.shape[y][x];
            }
        }
    }
    return rotated;
}

bool BlockGame::handle180Rotation() {
    if (currentTetromino.type == TetrominoType::O_PIECE) return false;

    Tetromino rotated = get180RotatedTetromino(currentTetromino);

    static constexpr auto kicks180 = std::to_array<Vec2>({
        { 0, 0 },
        { 1, 0 },
        { -1, 0 },
        { 0, 1 },
        { 0, -1 }
        });

    for (const auto& kick : kicks180) {
        Vec2 testPos = piecePosition + kick;
        if (isValidPosition(rotated, testPos)) {
            piecePosition = testPos;
            currentTetromino = rotated;
            lastActionWasRotation = true;
            lastKickOffset = kick;

            if (isLocking && lockResets < MAX_LOCK_RESETS) {
                lockStartTime = std::chrono::steady_clock::now();
                lockResets++;
            } else if (!isLocking && !testMove(0, 1)) {
                isLocking = true;
                lockStartTime = std::chrono::steady_clock::now();
                lockResets = 0;
            }
            return true;
        }
    }

    lastActionWasRotation = false;
    lastKickOffset = { 0, 0 };
    return false;
}

BlockGame::Tetromino BlockGame::get180RotatedTetromino(const Tetromino& original) {
    Tetromino rotated = original;
    rotated.rotationState = static_cast<RotationState>((original.rotationState + 2) % 4);

    int tempShape[4][4] = {{0}};
    const int size = original.dimension;

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            if (original.shape[y][x] != 0) {
                tempShape[size - 1 - y][size - 1 - x] = original.shape[y][x];
            }
        }
    }

    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            rotated.shape[y][x] = tempShape[y][x];
        }
    }

    return rotated;
}

void BlockGame::hardDrop() {
    int dropDistance = 0;
    Vec2 testPos = piecePosition;
    while (isValidPosition(currentTetromino, {testPos.x, testPos.y + 1})) {
        testPos.y++;
        dropDistance++;
    }

    if (dropDistance > 0) {
        piecePosition = testPos;
        score += dropDistance * 2;
    }

    mergeTetromino();

    int lines = clearLinesAndScore(lastMoveWasTSpin, lastMoveWasMiniTSpin);
    spawnTetromino(false);
}

void BlockGame::mergeTetromino() {
    bool isTSpin = false;
    bool isMini = false;
    if (currentTetromino.type == TetrominoType::T_PIECE && lastActionWasRotation) {
        isTSpin = checkTSpinCorners();
        const auto& kicks = (currentTetromino.type == TetrominoType::I_PIECE) ? srsKickDataI : srsKickDataJLSTZ;
        int kickIndex = -1;
        isMini = false;
        lastMoveWasTSpin = isTSpin;
        lastMoveWasMiniTSpin = isMini;
    } else {
        lastMoveWasTSpin = false;
        lastMoveWasMiniTSpin = false;
    }


    for (int y = 0; y < currentTetromino.dimension; ++y) {
        for (int x = 0; x < currentTetromino.dimension; ++x) {
            if (currentTetromino.shape[y][x] != 0) {
                int boardX = static_cast<int>(piecePosition.x + x);
                int boardY = static_cast<int>(piecePosition.y + y);
                if (boardY >= 0 && boardY < TOTAL_BOARD_HEIGHT && boardX >= 0 && boardX < BOARD_WIDTH) {
                    board[boardY][boardX] = currentTetromino.shape[y][x];
                } else {
                    Logger::Warn("BlockGame: Attempted to merge piece out of bounds at %d, %d", boardX, boardY);
                }
            }
        }
    }

    playSound("note.bd");

    isLocking = false;
    lastActionWasRotation = false;
}


bool BlockGame::checkTSpinCorners() {
    Vec2 corners[] = {
        { piecePosition.x + 0, piecePosition.y + 0 }, // Top-left
        { piecePosition.x + 2, piecePosition.y + 0 }, // Top-right
        { piecePosition.x + 0, piecePosition.y + 2 }, // Bottom-left
        { piecePosition.x + 2, piecePosition.y + 2 } // Bottom-right
    };

    int occupiedCount = 0;
    for (const auto& cornerPos : corners) {
        int boardX = static_cast<int>(cornerPos.x);
        int boardY = static_cast<int>(cornerPos.y);

        if (boardX < 0 || boardX >= BOARD_WIDTH || boardY < 0 || boardY >= TOTAL_BOARD_HEIGHT ||
            (boardY >= 0 && board[boardY][boardX] != 0)) {
            occupiedCount++;
        }
    }

    return occupiedCount >= 3;
}

int BlockGame::clearLinesAndScore(bool tspin, bool miniTspin) {
    int linesClearedThisTurn = 0;
    std::vector<int> clearedLineIndices;
    auto now = std::chrono::steady_clock::now();

    for (int y = TOTAL_BOARD_HEIGHT - 1; y >= 0; --y) {
        bool lineFull = true;
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (board[y][x] == 0) {
                lineFull = false;
                break;
            }
        }
        if (lineFull) {
            linesClearedThisTurn++;
            clearedLineIndices.push_back(y);
        }
    }

    currentClearIsDifficult = (linesClearedThisTurn >= 4 || tspin && linesClearedThisTurn != 0);

    if (linesClearedThisTurn > 0) {
        if (currentClearIsDifficult) {
            playSound("note.bit");
        } else {
            playSound("note.pling");
        }

        int destY = TOTAL_BOARD_HEIGHT - 1;
        for (int srcY = TOTAL_BOARD_HEIGHT - 1; srcY >= 0; --srcY) {
            bool wasCleared = false;
            for (int cy : clearedLineIndices) if (srcY == cy) {
                wasCleared = true;
                break;
            }
            if (!wasCleared) {
                if (destY != srcY) { for (int x = 0; x < BOARD_WIDTH; ++x) board[destY][x] = board[srcY][x]; }
                destY--;
            }
        }
        while (destY >= 0) {
            for (int x = 0; x < BOARD_WIDTH; ++x) board[destY][x] = 0;
            destY--;
        }
    }

    int scoreGained = 0;
    bool applyB2B = false;

    if (linesClearedThisTurn > 0) {
        comboCount++;
        comboUpdateTime = now;
        // TETR.IO Combo Formula: 50 * (combo - 1) * level
        if (comboCount >= 2) {
            scoreGained += 50 * (comboCount - 1) * level;
        }
    } else {
        comboCount = 0;
    }

    if (currentClearIsDifficult) {
        if (lastClearWasDifficult) {
            applyB2B = true;
            b2bCount++;
            b2bUpdateTime = now;
        } else {
            b2bCount = 1;
            b2bUpdateTime = now;
        }
    } else if (linesClearedThisTurn > 0) {
        b2bCount = 0;
    }

    int baseScore = 0;
    if (tspin) {
        // T-Spin Scores (TETR.IO guideline)
        if (miniTspin) {
            // Mini T-Spin
            if (linesClearedThisTurn == 0) baseScore = 100;
            else if (linesClearedThisTurn == 1) baseScore = 200;
            else if (linesClearedThisTurn == 2) baseScore = 400;
        }
        else {
            // Regular T-Spin
            if (linesClearedThisTurn == 0) baseScore = 400;
            else if (linesClearedThisTurn == 1) baseScore = 800;
            else if (linesClearedThisTurn == 2) baseScore = 1200;
            else if (linesClearedThisTurn == 3) baseScore = 1600;
        }
        if (miniTspin) {
            tSpinText = L"MINI T-SPIN";
        }
        else {
            if (linesClearedThisTurn == 1) tSpinText = L"T-SPIN SINGLE";
            else if (linesClearedThisTurn == 2) tSpinText = L"T-SPIN DOUBLE";
            else if (linesClearedThisTurn == 3) tSpinText = L"T-SPIN TRIPLE";
        }
        tSpinDisplayTime = std::chrono::steady_clock::now();
    }
    else {
        // Standard Line Clears
        if (linesClearedThisTurn == 1) baseScore = 100; // Single
        else if (linesClearedThisTurn == 2) baseScore = 300; // Double
        else if (linesClearedThisTurn == 3) baseScore = 500; // Triple
        else if (linesClearedThisTurn >= 4) baseScore = 800; // Tetris (4 lines)
        tSpinText = L"";
    }

    if (linesClearedThisTurn == 1) lineClearText = L"SINGLE";
    else if (linesClearedThisTurn == 2) lineClearText = L"DOUBLE";
    else if (linesClearedThisTurn == 3) lineClearText = L"TRIPLE";
    else if (linesClearedThisTurn >= 4) lineClearText = L"TETRIS";
    else lineClearText = L"";
    if (linesClearedThisTurn > 0) lineClearDisplayTime = std::chrono::steady_clock::now();

    baseScore *= level;

    if (applyB2B) {
        baseScore = static_cast<int>(baseScore * 1.5f);
    }

    scoreGained += baseScore;

    score += scoreGained;

    if (linesClearedThisTurn > 0) {
        linesCleared += linesClearedThisTurn;
        int nextLevelRequirement = level * 10;
        if (linesCleared >= nextLevelRequirement) {
            level++;
            playSound("random.levelup");
        }
    }

    if (linesClearedThisTurn > 0 || tspin) {
        lastClearWasDifficult = currentClearIsDifficult;
    }


    lastMoveWasTSpin = false;
    lastMoveWasMiniTSpin = false;

    return linesClearedThisTurn;
}

void BlockGame::onRenderHUDModules(Event& evGeneric) {
    DrawHUDModulesEvent& ev = reinterpret_cast<DrawHUDModulesEvent&>(evGeneric);
    ev.setCancelled(true);
}

bool BlockGame::isKeyDown(int vkCode) {
    return Latite::getKeyboard().isKeyDown(vkCode);
}

void BlockGame::playSound(const std::string& soundId) {
    if (std::get<BoolValue>(audio)) {
        util::PlaySoundUI(soundId);
    }
}