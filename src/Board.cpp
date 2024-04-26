//
// Created by cew05 on 24/04/2024.
//

#include "Board.h"

Board::Board() {
    // Create Textures
    if ((tileTextures[0] = IMG_LoadTexture(window.renderer, blackPath.c_str())) == nullptr) {
        LogError("Failed to obtain texture", SDL_GetError(), false);
    }
    if ((tileTextures[1] = IMG_LoadTexture(window.renderer, whitePath.c_str())) == nullptr) {
        LogError("Failed to obtain texture", SDL_GetError(), false);
    }

    CreateBoardTexture();

    // Create labels for rows
    for (int r = 0; r < rows; r++) {
        row_labels[r] = (char)r;
    }

    // Create labels for columns
    for (int c = 0; c < columns; c++) {
        col_labels[c] = char('A' + c);
    }
}

int Board::CreateBoardTexture() {
    // Set sizes for rect
    SDL_GetWindowSize(window.window, &boardRect.w, &boardRect.h);
    // determine minimum dimension to act as side length
    boardRect.w = boardRect.h = std::min(boardRect.w, boardRect.h);
    tileHeight = tileWidth = boardRect.h/8;

    // Create the texture to compile into
    SDL_DestroyTexture(boardTexture);
    boardTexture = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tileWidth * rows , tileHeight * rows);
    if (boardTexture == nullptr) {
        LogError("Failed to create Board Texture", SDL_GetError(), false);
        return -1;
    }

    if (SDL_SetRenderTarget(window.renderer, boardTexture) != 0) {
        LogError("Failed to set render target", SDL_GetError(), false);
        return -1;
    }

    // Create texture using the tile textures
    SDL_Rect tileRect {0, 0, tileWidth, tileHeight};
    int tT_index = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            SDL_RenderCopy(window.renderer, tileTextures[tT_index], nullptr, &tileRect);
            tileRect.x += tileRect.w;
            tT_index = (tT_index == 1) ? 0 : 1;
        }
        tT_index = (tT_index == 1) ? 0 : 1;
        tileRect.x = 0;
        tileRect.y += tileRect.h;
    }

    // reset render target to window
    SDL_SetRenderTarget(window.renderer, nullptr);
    return 0;
}

void Board::DisplayGameBoard() {
    SDL_RenderCopy(window.renderer, boardTexture, nullptr, &boardRect);
}