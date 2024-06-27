//
// Created by cew05 on 24/04/2024.
//

#include "include/Board.h"

Board::Board() {
    // Add rects for game board regions
    rm->NewResource({0, 0, minMenuWidth, minBoardWidth}, RectID::OPTIONS);
    rm->NewResource({minMenuWidth, 0, minBoardWidth, minBoardWidth}, RectID::BOARD);
    rm->NewResource({minMenuWidth + minBoardWidth, 0, minInfoWidth, minBoardWidth}, RectID::GAME_INFO);
    rm->NewResource({0, 0, 0, 0}, RectID::PROMO_MENU);

    // Construct rect for board tile
    SDL_Rect tileRect = {};
    GetTileRectFromPosition(tileRect, {'a', 1});
    rm->NewResource(tileRect, RectID::TILE);

    // Open textures
    tm->OpenTexture(BOARD_COMPILED);
    tm->OpenTexture(PROMO_WHITE_COMPILED);
    tm->OpenTexture(PROMO_BLACK_COMPILED);
    tm->OpenTexture(TextureID(BOARD_BASE_SECONDARY + BOARD_STYLE.second));
}

int Board::CreateBoardTexture() {
    // Open required textures
    tm->OpenTexture(TextureID(BOARD_BASE + BOARD_STYLE.second));
    tm->OpenTexture(TextureID(WHITE_TILE + BOARD_STYLE.second));
    tm->OpenTexture(TextureID(BLACK_TILE + BOARD_STYLE.second));

    TTF_Font* font = TTF_OpenFont("../Resources/Fonts/CF/TCFR.ttf", 100);

    // Get board tile dimensions, and board rect
    SDL_Rect boardRect;
    SDL_Rect tileRect;
    rm->FetchResource(boardRect, RectID::BOARD);

    // Create temp textures to draw to
    SDL_Texture* tempTexture;
    SDL_Texture* boardTexture = SDL_CreateTexture(window.renderer,
                                                  SDL_PIXELFORMAT_RGBA8888,
                                                  SDL_TEXTUREACCESS_TARGET,
                                                  boardRect.w, boardRect.h);
    if (boardTexture == nullptr) {
        LogError("Failed to create boardTexture", SDL_GetError(), false);
        return -1;
    }

    // Set the render target to the newly created board texture
    if (SDL_SetRenderTarget(window.renderer, boardTexture) != 0) {
        LogError("Failed to set render target", SDL_GetError(), false);
        return -1;
    }

    // Draw Board Base texture
    tempTexture = tm->AccessTexture(TextureID(BOARD_BASE + BOARD_STYLE.second));
    if (SDL_RenderCopy(window.renderer, tempTexture, nullptr, nullptr) != 0) {
        printf("%s\n", TextureManager::GetIssueString(tm->GetIssue()).c_str());
        LogError("Failed to copy base texture", SDL_GetError(), false);
        return -1;
    }

    // Shrink boardRect to create the gameboard within

    // Create board grid using the tile textures
    bool whiteTile = false;

    // Determine top left tile size and position
    tileRect = {0, 0,
                int(((double)boardRect.w * (1-2*boardBorder)) / columns),
                int(((double)boardRect.h * (1-2*boardBorder)) / rows)};
    tileRect.x = (boardRect.w - (columns*tileRect.w)) / 2;
    tileRect.y = (boardRect.h - (rows*tileRect.h)) / 2;

    // Store tile
    topLeftTile = {tileRect.x, tileRect.y, tileRect.w, tileRect.h};

    // Now draw board grid pattern
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            // Draw tile
            tempTexture = tm->AccessTexture(TextureID((whiteTile ? WHITE_TILE : BLACK_TILE) + BOARD_STYLE.second));
            SDL_RenderCopy(window.renderer, tempTexture, nullptr, &tileRect);

            // move rect position
            tileRect.x += tileRect.w;
            whiteTile = !whiteTile;
        }
        // move up to next row
        whiteTile = !whiteTile;
        tileRect.x = (boardRect.w - (columns*tileRect.w)) / 2;
        tileRect.y += tileRect.h;
    }

    // Get font for labels
    if (font == nullptr) {
        // exit early on failure
        LogError("Failed to load font", SDL_GetError(), false);
        return -1;
    }

    // Reset tile position, set to BL tile and indent
    tileRect.x = (boardRect.w - (columns*tileRect.w)) / 2;
    tileRect.y = (boardRect.h - (rows*tileRect.h)) / 2;
    tileRect.x += int((double)tileRect.w * tile_borderWidth);
    tileRect.y += int((double)tileRect.y * tile_borderHeight);
    tileRect.y += tileRect.h * (rows-1);

    std::string label = " ";
    for (int axis = 0; axis < 2; axis++) {
        // Axis == 0 -> rows
        for (int lIndex = 0; lIndex < ((axis == 0) ? rows : columns); lIndex++) {
            label[0] = (axis == 0) ? char('1' + lIndex) : char('a' + lIndex);
            SDL_Rect labelRect = {tileRect.x, tileRect.y, int(tileRect.w/2), int(tileRect.h/2)};
            SDL_Surface* surface;

            // Get label texture
            surface = TTF_RenderText_Blended(font, label.c_str(), {255, 255, 255});
            tempTexture = SDL_CreateTextureFromSurface(window.renderer, surface);
            SDL_FreeSurface(surface);

            // fetch ratio between height of label texture and height of tile
            std::pair<int, int> labelSize;
            SDL_QueryTexture(tempTexture, nullptr, nullptr, &labelSize.first, &labelSize.second);
            float heightRatio = (float)labelSize.second / (float)labelRect.h;

            // set height and width to be proportional
            labelRect.h = labelRect.h;
            labelRect.w = int((float)labelSize.first / heightRatio);

            // adjust for space above / below the character
            if (axis == 0) labelRect.y -= int(double(TTF_FontHeight(font) - TTF_FontAscent(font)) / heightRatio);
            else labelRect.y -= int((double)TTF_FontDescent(font) / heightRatio);

            // Adjust position of row label
            if (axis == 0) tileRect.y -= tileRect.h;
            else {
                tileRect.x += tileRect.w;
                labelRect.x -= labelRect.w;
                labelRect.y -= labelRect.h;
            }

            // Draw label
            SDL_RenderCopy(window.renderer, tempTexture, nullptr, &labelRect);
        }

        // Reset tile position, set to BL tile and indent
        tileRect.x = (boardRect.w - (columns*tileRect.w)) / 2;
        tileRect.y = (boardRect.h - (rows*tileRect.h)) / 2;
        tileRect.x += tileRect.w - int((double)tileRect.w * tile_borderWidth);
        tileRect.y += tileRect.h - int((double)tileRect.h * tile_borderHeight);
        tileRect.y += tileRect.h * (rows-1);
    }

    // close textures
    tm->CloseTexture(TextureID(BOARD_BASE + BOARD_STYLE.second));
    tm->CloseTexture(TextureID(WHITE_TILE + BOARD_STYLE.second));
    tm->CloseTexture(TextureID(BLACK_TILE + BOARD_STYLE.second));
    TTF_CloseFont(font);

    // Update compiled board texture
    tm->UpdateTexture(boardTexture, BOARD_COMPILED);

    // reset render target to window
    SDL_SetRenderTarget(window.renderer, nullptr);
    return 0;
}

bool Board::CreatePromoMenuTexture() {
    /*
     * need to add images of the (teamcolour) bishop, knight, rook, queen onto the menu texture for the user to
     * select from
     */

    // Load textures
    tm->OpenTexture(TextureID(PROMO_BASE + BOARD_STYLE.second));
    for (int c = 0; c < 2; c++) {
        tm->OpenTexture(TextureID(WHITE_QUEEN + PIECE_STYLE.second + c));
        tm->OpenTexture(TextureID(WHITE_ROOK + PIECE_STYLE.second + c));
        tm->OpenTexture(TextureID(WHITE_BISHOP + PIECE_STYLE.second + c));
        tm->OpenTexture(TextureID(WHITE_KNIGHT + PIECE_STYLE.second + c));
    }

    // Temp vars
    SDL_Texture* tempTexture;
    SDL_Rect promoRect = {0, 0, 200, 56};
    SDL_Rect iconRect;

    for (int col = 0; col < 2; col++){
        // Create targetable texture for renderer to draw to
        SDL_Texture *promoTexture = SDL_CreateTexture(window.renderer,
                                                      SDL_PIXELFORMAT_RGBA8888,
                                                      SDL_TEXTUREACCESS_TARGET,
                                                      promoRect.w, promoRect.h);
        if (promoTexture == nullptr) {
            LogError("Failed to create promotion menu TextureID", SDL_GetError(), false);
            return false;
        }

        // Set the render target to the newly created texture
        if (SDL_SetRenderTarget(window.renderer, promoTexture) != 0) {
            LogError("Failed to set render target", SDL_GetError(), false);
            return false;
        }

        // Draw base
        tempTexture = tm->AccessTexture(PROMO_BASE);
        if (SDL_RenderCopy(window.renderer, tempTexture, nullptr, nullptr) != 0) {
            LogError("Failed to draw promoMenu base texture", SDL_GetError(), false);
            return false;
        }

        // Draw icons
        iconRect = {promoRect.w / 25, promoRect.h / 7, promoRect.w * 5/25, promoRect.h * 5/7};
        TextureID pieceIDs[4] = {WHITE_QUEEN, WHITE_ROOK, WHITE_BISHOP, WHITE_KNIGHT};
        RectID rectIDs[4] = {RectID::PROMO_QUEEN, RectID::PROMO_ROOK, RectID::PROMO_BISHOP, RectID::PROMO_KNIGHT};
        for (int id = 0; id < 4; id++) {
            // fetch icon texture
            if ((tempTexture = tm->AccessTexture(TextureID(pieceIDs[id] + col + PIECE_STYLE.second))) == nullptr) {
                std::string issue = "Failed to load icon for promoMenus";
                LogError(issue, SDL_GetError(), false);
                return false;
            }

            // draw icon
            if (SDL_RenderCopy(window.renderer, tempTexture, nullptr, &iconRect) != 0) {
                LogError("Failed to draw icon texture to promoMenus", SDL_GetError(), false);
                return false;
            }

            // add iconRect to rectCollection, move to next icon position
            rm->NewOrUpdateResource(iconRect, rectIDs[id]);
            iconRect.x += iconRect.w * 6/5;
        }

        // add promoMenu texture to texture collection
        tm->UpdateTexture(promoTexture, (col == 0) ? PROMO_WHITE_COMPILED : PROMO_BLACK_COMPILED);
    }

    // Reset render target
    SDL_SetRenderTarget(window.renderer, nullptr);

    // Reposition the promoMenus rect to centre of boardRect
    SDL_Rect boardRect;
    rm->FetchResource(boardRect, RectID::BOARD);
    promoRect.x = boardRect.x + boardRect.w/2 - promoRect.w/2;
    promoRect.y = boardRect.h/2 - promoRect.h/2;

    // update stored promoRect
    rm->ChangeResource(promoRect, RectID::PROMO_MENU);

    // Load textures
    tm->CloseTexture(TextureID(PROMO_BASE + BOARD_STYLE.second));
    for (int c = 0; c < 2; c++) {
        tm->CloseTexture(TextureID(WHITE_QUEEN + PIECE_STYLE.second + c));
        tm->CloseTexture(TextureID(WHITE_ROOK + PIECE_STYLE.second + c));
        tm->CloseTexture(TextureID(WHITE_BISHOP + PIECE_STYLE.second + c));
        tm->CloseTexture(TextureID(WHITE_KNIGHT + PIECE_STYLE.second + c));
    }

    // success
    return true;
}

void Board::DisplayPromoMenu(Piece* _promotingPiece) {
    int col = _promotingPiece->GetPieceInfoPtr()->colID == 'W' ?  PROMO_WHITE_COMPILED : PROMO_BLACK_COMPILED;
    SDL_Texture* displayMenu = tm->AccessTexture(col);
    SDL_Rect rect;
    rm->FetchResource(rect, RectID::PROMO_MENU);

    SDL_RenderCopy(window.renderer, displayMenu, nullptr, &rect);
}

void Board::DisplayGameBoard() {
    // temp texture var
    SDL_Texture* texture;
    SDL_Rect rect;

    // DisplayToggle Board background
    texture = tm->AccessTexture(BOARD_COMPILED);
    rm->FetchResource(rect, RectID::BOARD);
    SDL_RenderCopy(window.renderer, texture, nullptr, &rect);

    // display menu background
    texture = tm->AccessTexture(BOARD_BASE_SECONDARY);
    rm->FetchResource(rect, RectID::OPTIONS);
    SDL_RenderCopy(window.renderer, texture, nullptr, &rect);

    // display game info background
    texture = tm->AccessTexture(BOARD_BASE_SECONDARY);
    rm->FetchResource(rect, RectID::GAME_INFO);
    SDL_RenderCopy(window.renderer, texture, nullptr, &rect);
}

std::string Board::GetPromoMenuInput() {
    /*
     * Returns string name on selection of Knight, Bishop, Rook or Queen respectively
     */

    // no click made yet
    if (!mouse.IsUnheldActive()) return "noinput";

    RectID rectIDs[4] = {RectID::PROMO_QUEEN, RectID::PROMO_ROOK, RectID::PROMO_BISHOP, RectID::PROMO_KNIGHT};
    SDL_Rect promoMenuRect;
    rm->FetchResource(promoMenuRect, RectID::PROMO_MENU);

    for (auto& rectID : rectIDs) {
        SDL_Rect rect;
        rm->FetchResource(rect, rectID);
        rect.x += promoMenuRect.x;
        rect.y += promoMenuRect.y;

        if (mouse.UnheldClick(rect)) {
            std::string pieceName;
            switch (rectID){
                case RectID::PROMO_QUEEN: pieceName = "Queen"; break;
                case RectID::PROMO_ROOK: pieceName = "Rook"; break;
                case RectID::PROMO_BISHOP: pieceName = "Bishop"; break;
                case RectID::PROMO_KNIGHT: pieceName = "Knight"; break;
                default: break;
            }

            return pieceName;
        }
    }

    // click made, but not on menu. exit menu
    return "";
}

void Board::GetTileDimensions(int& _w, int& _h) const {
    _w = topLeftTile.w;
    _h = topLeftTile.h;
}

void Board::GetMinDimensions(int& _w, int& _h) const {
    _w = minBoardWidth + minInfoWidth + minMenuWidth;
    _h = minBoardWidth;
}

void Board::GetRowsColumns(int &_rows, int &_cols) {
    _rows = rows;
    _cols = columns;
}

Pair<int> Board::GetRowsColumns() {
    return {rows, columns};
}

void Board::GetBoardBLPosition(int& _x, int& _y) const {
    SDL_Rect boardRect;
    rm->FetchResource(boardRect, RectID::BOARD);

    _x = boardRect.x + topLeftTile.x;
    _y = boardRect.y + topLeftTile.y + (topLeftTile.h*(rows-1));
}

void Board::GetTileRectFromPosition(SDL_Rect &rect, Position<char, int> position) const {
    // Get rect of a1
    GetTileDimensions(rect.w, rect.h);
    GetBoardBLPosition(rect.x, rect.y);

    // now move x and y to position
    SDL_Rect boardRect;
    rm->FetchResource(boardRect, RectID::BOARD);
    rect.x += (rect.w * (std::tolower(position.x)-'a'));
    rect.y -= (rect.h * (position.y - 1));
}

void Board::GetBorderedRectFromPosition(SDL_Rect &_rect, Position<char, int> _position) const {
    // Get rect of a tile on a position without border
    GetTileRectFromPosition(_rect, _position);

    // move the tl position of _rect by borderSize of tileWidth/tileHeight to centre the _rect
    _rect.x += int((double)_rect.w * tile_borderWidth);
    _rect.y += int((double)_rect.h * tile_borderHeight);

    // now apply 10% border to the rect
    _rect.w = int((double)_rect.w * (1-2*tile_borderWidth));
    _rect.h = int((double)_rect.h * (1-2*tile_borderHeight));
}

/*
 * SETTERS
 */

void Board::FillToBounds(int _w, int _h) {
    printf("w: %d h: %d\n", _w, _h);
    /*
     * section min size / sum of minimums = the multiplier of the given _width or _height values to determine the
     * sections new size. Ratio of newBoardSize / minBoardSize is used to alter the other sections sizes.
     */

    double ratio;
    int sumMinWidth = minBoardWidth + minMenuWidth + minInfoWidth;

    // Determine size of the main board and ensure height of screen is not exceeded

    // Ensure height of screen isn't exceeded
    ratio = (double)minBoardWidth / sumMinWidth;
    int boardHeight = int(_w * ratio);
    if (boardHeight > _h) boardHeight = _h;
    ratio = (double)boardHeight / minBoardWidth;

    // Resize rects
    RectID rectIDs[3] {RectID::OPTIONS, RectID::BOARD, RectID::GAME_INFO};
    int minWidth[3] {minMenuWidth, minBoardWidth, minInfoWidth};
    for (int id = 0; id < 3; id++) {
        SDL_Rect rect;
        rm->FetchResource(rect, rectIDs[id]);
        rect.h = boardHeight;
        rect.w = int(minWidth[id] * ratio);

        // Reposition rects due to new sizes
        rect.y = 0;
        rect.x = 0;

        if (id > 0) {
            SDL_Rect prevRect;
            rm->FetchResource(prevRect, rectIDs[id-1]);
            rect.x = prevRect.x + prevRect.w;
        }

        // Update stored rect
        rm->ChangeResource(rect, rectIDs[id]);
    }
}

/*
 * GAMEPLAY RECORDING
 */

bool Board::GameDataDirectoryExists() {
    /*
     * Check to ensure that the directory to house GameData (path specified by string gameDataDirPath) exists.
     * If not, create the directory. If this fails, return false.
     */

    // Check to ensure dir housing GameData exists
    if (!std::filesystem::exists(gameDataDirPath)) {
        printf("no GameData dir found, creating.\n");
        if (!std::filesystem::create_directory(gameDataDirPath)) {
            printf("failed to create GameData dir, returning false.");
            return false;
        }
    }

    // directory exists
    return true;
}

void Board::ClearExcessGameFiles() {
    /*
     * Removes excess game data files. Excess files when > 10 files. Oldest dated files removed first. Files with
     * unreadable date names are destroyed first under assumption that they are of old unsupported format / processes.
     */

    std::vector<AsymPair<std::string, time_t>> pathTimes;

    // Fetch all contents of GameData dir and convert paths to fileTimes in list
    for (const auto& gameData : std::filesystem::directory_iterator(gameDataDirPath)) {
        AsymPair<std::string, time_t> pathTime;
        std::string pathTimeString;
        struct tm structtm {};

        pathTime.a = gameData.path().string();

        // now get timeString by removing the path to the folder + copy value
        //remove folder path string leaving just date + copy
        pathTimeString = pathTime.a.substr(gameDataDirPath.length()+1, std::string::npos);

        // ensure folder name is long enough to house the full time string
        if (pathTimeString.length() >= timeStringFormat.size() - 1) {
            pathTimeString.erase(timeStringFormat.size() -1, std::string::npos);

            // now turn into structtm, >> is highlighted as an error yet works???
            std::istringstream ss(pathTimeString);
            ss >> std::get_time(&structtm, timeFormat.c_str());

            // turn into time_t which can be stored then compared
            pathTime.b = mktime(&structtm);

        } else {
            // default to time 0
            pathTime.b = 0;
        }

        pathTimes.push_back(pathTime);
    }

    // sort by time oldest -> newest
    std::sort(pathTimes.begin(), pathTimes.end(),
              [&](const AsymPair<std::string, time_t>& pathTimeA,const AsymPair<std::string, time_t>& pathTimeB){
        return (pathTimeA.b < pathTimeB.b);
    });

    // remove folders
    uintmax_t nRemoved = 0;
    std::vector<std::string> dirsRemoved {};
    for (auto pt = pathTimes.begin(); pt < pathTimes.end();) {
        if (pathTimes.size() > 10) {
            nRemoved += std::filesystem::remove_all(pt->a);
            dirsRemoved.push_back(pt->a);

            pt = pathTimes.erase(pt);
            continue;
        }
        pt++;
    }

    // summary
    printf("Removed directories:\n");
    for (const auto& dir : dirsRemoved) {
        printf("%s\n", dir.c_str());
    }
    printf("Removed %ju total files/Directories\n\n", nRemoved);
}

bool Board::CreateGameFiles() {
    /*
     * Create files to store the game data for the game about to be played. This includes starting positions of pieces,
     * and the movelist in ACN.
     */

    // Path string for the game data folder in GameData dir
    std::string gameDirPath = gameDataDirPath + "/";

    // Get date:
    char timeChar[timeStringFormat.size()];
    time_t t = time(nullptr);
    std::strftime(timeChar, sizeof(timeChar), timeFormat.c_str(), localtime(&t));
    printf("Date_Time : %s\n", timeChar);
    gameDirPath += timeChar;

    // Check to ensure dir using date (and/or copy number) doesn't already exist. If does exist, keep adding 1 to
    // dupe indicator on end until unique name.
    std::string gameDirPathDupe = gameDirPath;
    int dupeIndicator = 1;
    while (std::filesystem::exists(gameDirPathDupe)) {
        gameDirPathDupe = gameDirPath + "_" + std::to_string(dupeIndicator);
        dupeIndicator += 1;
    }
    gameDirPath = gameDirPathDupe;

    // Create dir for game data using path
    if (!std::filesystem::create_directory(gameDirPath)) {
        printf("failed to create GameData_Time_Copy dir, returning false.");
        return false;
    }

    // Create file to house ACN of game moves
    moveListFilePath = gameDirPath + "/ACNmovelist.txt";
    std::ofstream file(moveListFilePath);
    if (!file.good()) {
        file.close();
        return false;
    }
    file.close();

    // Create file to house ACN of piece start positions
    startPosFilePath = gameDirPath + "/ACNstartpos.csv";
    file.open(startPosFilePath);
    if (!file.good()) {
        file.close();
        return false;
    }
    file.close();

    return true;
}

bool Board::WriteStartPositionsToFile(const std::vector<Piece *> &_allPieces) {
    std::fstream spFile(startPosFilePath.c_str());
    if (!spFile.good()) {
        spFile.close();
        return false;
    }

    for (auto piece : _allPieces) {
        Piece_Info* pInfo = piece->GetPieceInfoPtr();
        std::string pInfoStr; pInfoStr.append(&pInfo->colID);
        pInfoStr += "," + pInfo->name + "," + pInfo->gamepos.x + "," + std::to_string(pInfo->gamepos.y);
        spFile << pInfoStr << std::endl;
    }

    spFile.close();
    return true;
}

bool Board::WriteMoveToFile(const std::string& _move) {
    std::fstream mlFile(moveListFilePath.c_str(), std::ios::app);
    if (!mlFile.good()) {
        mlFile.close();
        return false;
    }

    if (numEots == 0) mlFile << std::endl << turn << ". ";
    mlFile << _move << " ";

    mlFile.close();
    return true;
}

void Board::IncrementTurn() {
    numEots += 1;
    if (numEots == 2) {
        numEots = 0;
        turn += 1;
    }
}
