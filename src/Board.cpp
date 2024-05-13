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
    if ((boardBases[0] = IMG_LoadTexture(window.renderer, basePath.c_str())) == nullptr) {
        LogError("Failed to obtain pieceTexture", SDL_GetError(), false);
    }
    if ((boardBases[1] = IMG_LoadTexture(window.renderer, secondBasePath.c_str())) == nullptr) {
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

    // Add in Board Base pieceTexture:
    SDL_Rect tileRect {0, 0, boardRect.w, boardRect.h};
    SDL_RenderCopy(window.renderer, boardBases[0], nullptr, &tileRect);

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

    // Create board base texture
    if ((boardBases[0] = IMG_LoadTexture(window.renderer, basePath.c_str())) == nullptr) {
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
        row_label_rects[r] = {boardRect.x + boardRect.w / 20 - w / 2, boardRect.h * 9/10  - tileHeight- (tileHeight * r), w, h};
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
        col_label_rects[c] = {boardRect.x + boardRect.w * 1/10 + tileWidth * c + tileWidth/2 - w/2, boardRect.h * 9/10, w, h};
    }

    // reset render target to window
    SDL_SetRenderTarget(window.renderer, nullptr);
    TTF_CloseFont(labelFont);
    return 0;
}

void Board::DisplayGameBoard() {
    // Display Board background
    SDL_RenderCopy(window.renderer, boardTexture, nullptr, &boardRect);

    // Display row labels
    for (int r = 0; r < rows; r++) {
        SDL_RenderCopy(window.renderer, row_label_textures[r], nullptr, &row_label_rects[r]);
    }

    // Display colID labels
    for (int c = 0; c < columns; c++) {
        SDL_RenderCopy(window.renderer, col_label_textures[c], nullptr, &col_label_rects[c]);
    }

    // display menu background
    SDL_RenderCopy(window.renderer, boardBases[1], nullptr, &menuRect);

    // display game info background
    SDL_RenderCopy(window.renderer, boardBases[1], nullptr, &gameInfoRect);
}

void Board::GetTileRowsColumns(int &_rows, int &_cols) {
    _rows = rows;
    _cols = columns;
}

void Board::GetBoardBLPosition(int& x, int& y) const {
    x = boardRect.x + boardRect.w/10;
    y = boardRect.h * 9/10 - tileHeight;
}

void Board::GetTileRectFromPosition(SDL_Rect &rect, Position<char, int> position) const {
    // set rect to originate from tl of the board's bottom left tile
    GetBoardBLPosition(rect.x, rect.y);

    // now add the position values to the rect values
    rect.x += (tileWidth * (position.x-'a'));
    rect.y -= (tileWidth * (position.y - 1));
    rect.w = tileWidth;
    rect.h = tileHeight;
}

void Board::GetBorderedRectFromPosition(SDL_Rect &rect, Position<char, int> position) const {
    // set rect to originate from tl of the board's bottom left tile
    GetBoardBLPosition(rect.x, rect.y);

    // now add the position values to the rect values
    rect.x += (tileWidth * (position.x-'a'));
    rect.y -= (tileWidth * (position.y - 1));
    rect.w = tileWidth;
    rect.h = tileHeight;

    // now apply 10% border to the rect
    rect.w = int((float)rect.w * (1 - (tile_borderWidth * 2)));
    rect.h = int((float)rect.h * (1 - (tile_borderHeight * 2)));

    // now move the tl of rect by 10% of tileWidth/tileHeight to centre the rect
    rect.x += int((float)tileWidth*tile_borderWidth);
    rect.y += int((float)tileHeight*tile_borderHeight);
}

bool Board::CreateTempGameFile() {
    // Get date:
    char timeChar[sizeof("yyyy-mm-ddThh:mm:ssZ")];
    time_t t = time(nullptr);
    std::strftime(timeChar, sizeof(timeChar), "%H_%M_%S_%d_%m_%Y", localtime(&t));
    std::string timeString = timeChar;

    std::string dirName = "Game_" + timeString;
    moveListFile = "../Temp/" + dirName + "_ACNmovelist.txt";

    std::ofstream file(moveListFile.c_str());
    if (!file.good()) {
        file.close();
        return false;
    }
    file.close();

    startPosFile = "../Temp/" + dirName + "_ACNstartpos.csv";
    file.open(startPosFile.c_str());
    if (!file.good()) {
        file.close();
        return false;
    }
    file.close();

    return true;
}

bool Board::WriteStartPositionsToFile(const std::vector<Piece *> &_allPieces) {
    std::fstream spFile(startPosFile.c_str());
    if (!spFile.good()) {
        spFile.close();
        return false;
    }

    for (auto piece : _allPieces) {
        Piece_Info* pInfo = piece->GetPieceInfoPtr();
        std::string pInfoStr = pInfo->color + "," + pInfo->name + "," + pInfo->gamepos.x + "," + std::to_string(pInfo->gamepos.y);
        spFile << pInfoStr << std::endl;
    }

    spFile.close();

    return true;
}
