//
// Created by cew05 on 24/04/2024.
//

#include "src_headers/Board.h"

Board::Board() {
    // Create Textures
    if ((tileTextures[0] = IMG_LoadTexture(window.renderer, blackPath.c_str())) == nullptr) {
        LogError("Failed to obtain pieceTexture", SDL_GetError(), false);
    }
    if ((tileTextures[1] = IMG_LoadTexture(window.renderer, whitePath.c_str())) == nullptr) {
        LogError("Failed to obtain pieceTexture", SDL_GetError(), false);
    }
    if ((boardBase = IMG_LoadTexture(window.renderer, basePath.c_str())) == nullptr) {
        LogError("Failed to obtain pieceTexture", SDL_GetError(), false);
    }

    // Create labels for rows
    for (int r = 0; r < rows; r++) {
        row_labels[r] = 1 + r;
    }

    // Create labels for columns
    for (int c = 0; c < columns; c++) {
        col_labels[c] = char('A' + c);
    }
}

int Board::CreateBoardTexture() {
    // Set sizes for rect
    // determine minimum dimension to act as side length
    SDL_GetWindowSize(window.window, &boardRect.w, &boardRect.h);
    boardRect.w = boardRect.h = std::min(boardRect.w, boardRect.h);

    // determine tile size
    tileWidth = int(0.8 * (float)boardRect.w / columns);
    tileHeight = int(0.8 * (float)boardRect.h / rows);

    // Create the pieceTexture to compile into
    SDL_DestroyTexture(boardTexture);
    boardTexture = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                     boardRect.w, boardRect.h);
    if (boardTexture == nullptr) {
        LogError("Failed to create Board Texture", SDL_GetError(), false);
        return -1;
    }

    if (SDL_SetRenderTarget(window.renderer, boardTexture) != 0) {
        LogError("Failed to set render target", SDL_GetError(), false);
        return -1;
    }

    // add in Board Base pieceTexture:
    SDL_Rect tileRect {0, 0, boardRect.w, boardRect.h};
    SDL_RenderCopy(window.renderer, boardBase, nullptr, &tileRect);

    // Create pieceTexture using the tile textures
    tileRect = {int(boardRect.w * 0.1), int(boardRect.h * 0.1), tileWidth, tileHeight};
    int tT_index = 1;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            SDL_RenderCopy(window.renderer, tileTextures[tT_index], nullptr, &tileRect);
            tileRect.x += tileRect.w;
            tT_index = (tT_index == 1) ? 0 : 1;
        }
        tT_index = (tT_index == 1) ? 0 : 1;
        tileRect.x = int(boardRect.w * 0.1);
        tileRect.y += tileRect.h;
    }

    // Create board base pieceTexture
    if ((boardBase = IMG_LoadTexture(window.renderer, basePath.c_str())) == nullptr) {
        LogError("Failed to create piece pieceTexture", SDL_GetError(), false);
        return -1;
    }

    // Now create Row / Column label textures
    TTF_Font* labelFont = TTF_OpenFont("../Resources/Fonts/CF/TCFR.ttf", 100);
    if (labelFont == nullptr) {
        LogError("Failed to create font", SDL_GetError(), false);
        return -1;
    }

    // Create labels for rows
    for (int r = 0; r < rows; r++) {
        // Create pieceTexture
        std::string label = std::to_string(row_labels[r]);
        surface = TTF_RenderText_Blended(labelFont, label.c_str(), {255, 255, 255});
        row_label_textures[r] = SDL_CreateTextureFromSurface(window.renderer, surface);
        SDL_FreeSurface(surface);

        // Find ratio to reduce w/h equally
        int w, h;
        SDL_QueryTexture(row_label_textures[r], nullptr, nullptr, &w, &h);
        float ratio = (float)h / (float)tileHeight;

        // Set w/h
        h = int((float)h / ratio);
        w = int((float)w / ratio);

        // Create Rect
        row_label_rects[r] = {boardRect.w / 20 - w / 2, boardRect.h/10 + (tileHeight * r), w, h};
    }

    // Create labels for columns
    for (int c = 0; c < columns; c++) {
        // Create pieceTexture
        std::string label = std::string(1, col_labels[c]);
        surface = TTF_RenderText_Blended(labelFont, label.c_str(), {255, 255, 255});
        col_label_textures[c] = SDL_CreateTextureFromSurface(window.renderer, surface);
        SDL_FreeSurface(surface);

        // Find ratio to reduce w/h equally
        int w, h;
        SDL_QueryTexture(col_label_textures[c], nullptr, nullptr, &w, &h);
        float ratio = (float)h / (float)tileHeight;

        // Set w/h
        h = int((float)h / ratio);
        w = int((float)w / ratio);

        // Create Rect
        col_label_rects[c] = {boardRect.w * 1/10 + tileWidth * c + tileWidth/2 - w/2, boardRect.h * 9/10, w, h};
    }

    // reset render target to window
    SDL_SetRenderTarget(window.renderer, nullptr);
    TTF_CloseFont(labelFont);
    return 0;
}

void Board::DisplayGameBoard() {
    // Display Board
    SDL_RenderCopy(window.renderer, boardTexture, nullptr, &boardRect);

    // Display row labels
    for (int r = 0; r < rows; r++) {
        SDL_RenderCopy(window.renderer, row_label_textures[r], nullptr, &row_label_rects[r]);
    }

    // Display col labels
    for (int c = 0; c < columns; c++) {
        SDL_RenderCopy(window.renderer, col_label_textures[c], nullptr, &col_label_rects[c]);
    }
}



void Board::GetBoardTLPosition(int& x, int& y) const {
    x = tileWidth;
    y = tileHeight;
}