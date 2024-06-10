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

bool Board::CreatePromoMenuTexture() {
    /*
     * need to add images of the (teamcolour) bishop, knight, rook, queen onto the menu texture for the user to
     * select from
     */

    std::string col[2] = {"Black", "White"};
    int cIndex = 0;

    for (SDL_Texture* &menu : promoMenus) {
        // Remove prior contents if exist
        if (menu != nullptr) SDL_DestroyTexture(menu);

        // Create targetable texture for renderer to draw to
        menu = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,
                                 promoMenuRect.w, promoMenuRect.h);
        if (menu == nullptr) {
            LogError("Failed to create promoMenu texture", SDL_GetError(), false);
            return false;
        }

        // Change the rendering target
        SDL_SetRenderTarget(window.renderer, menu);

        // Now apply the base image
        promoMenuBase = IMG_LoadTexture(window.renderer, "../Resources/Menus/PromotionMenu.png");
        if (promoMenuBase == nullptr) {
            LogError("Failed to create promoMenu base texture", SDL_GetError(), false);
            return false;
        }

        // Draw base image to target
        SDL_RenderCopy(window.renderer, promoMenuBase, nullptr, nullptr);

        // Piece textures values
        SDL_Texture* pieceTexture;
        const int nPieces = 4;
        std::string path;

        // set offset values for icons
        const int offsetW = promoMenuRect.w / 25;
        const int offsetH = promoMenuRect.h / 7;
        for (auto &pir : promoIconRects) pir = {offsetW, offsetH, offsetW * 5, offsetH * 5};

        for (int i = 0; i < nPieces; i++) {
            // Determine path (dont even with the += it just stopped a warning about string append)
            path += "../Resources/" + pieceNames[i] += "/" + pieceNames[i] += "_" + col[cIndex] + "_" + PIECE_STYLE + ".png";

            // Fetch texture
            if ((pieceTexture = IMG_LoadTexture(window.renderer, path.c_str())) == nullptr) {
                std::string issue = "Failed to load " + pieceNames[i] + " icon for promoMenus";
                LogError(issue, SDL_GetError(), false);
                return false;
            }

            promoIconRects[i].x += i * 6 * offsetW;
            if (SDL_RenderCopy(window.renderer, pieceTexture, nullptr, &promoIconRects[i]) != 0) {
                LogError("Failed to add icon texture to promoMenus", SDL_GetError(), false);
                return false;
            }

            path = "";
        }

        cIndex++;
    }

    // Reset render target
    SDL_SetRenderTarget(window.renderer, nullptr);

    // Reposition the promoMenus rect
    promoMenuRect.x = menuRect.w + boardRect.w/2 - promoMenuRect.w/2;
    promoMenuRect.y = boardRect.h/2 - promoMenuRect.h/2;

    printf("x : %d y : %d w : % d h : %d\n", promoMenuRect.x, promoMenuRect.y, promoMenuRect.w, promoMenuRect.h);

    // success
    return true;
}

void Board::DisplayPromoMenu(Piece* _promotingPiece) {
    auto displayMenu = promoMenus[(_promotingPiece->GetPieceInfoPtr()->colID == 'B') ? 0 : 1];
    SDL_RenderCopy(window.renderer, displayMenu, nullptr, &promoMenuRect);
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

std::string Board::GetPromoMenuInput() {
    /*
     * Returns string name on selection of Knight, Bishop, Rook or Queen respectively
     */

    // no click made yet
    if (!mouse.IsUnheldActive()) return "noinput";

    for (int irIndex = 0; irIndex < 4; irIndex++ ) {
        SDL_Rect iconRect = promoIconRects[irIndex];
        iconRect.x += promoMenuRect.x;
        iconRect.y += promoMenuRect.y;

        if (mouse.UnheldClick(iconRect)) {
            printf("clicked on %s\n", pieceNames[irIndex].c_str());
            return pieceNames[irIndex];
        }
    }

    // click made, but not on menu. exit menu
    return "";
}


void Board::GetRowsColumns(int &_rows, int &_cols) {
    _rows = rows;
    _cols = columns;
}

Pair<int> Board::GetRowsColumns() {
    return {rows, columns};
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
    int sumMinWidth = minBoardSize + minMenuWidth + minInfoWidth;

    // Determine size of the main board and ensure height of screen is not exceeded
    ratio = (double)minBoardSize / sumMinWidth;
    boardRect.w = int(_w * ratio);
    if (boardRect.w > _h) {
        boardRect.w = _h;
        ratio = (double)boardRect.w / minBoardSize;
    }
    boardRect.h = boardRect.w;

    // Determine size of left menu board
    menuRect.h = boardRect.h;
    ratio = (double)menuRect.h / minBoardSize;
    menuRect.w = int(minMenuWidth * ratio);

    // Determine size of right game info board
    gameInfoRect.h = boardRect.h;
    ratio = (double)gameInfoRect.h / minBoardSize;
    gameInfoRect.w = int(minInfoWidth * ratio);

    /*
     * Reposition rects due to new sizes
     */

    // menuRect remains the same at 0,0
    boardRect.x = menuRect.w;
    gameInfoRect.x = boardRect.x + boardRect.w;

    // now update the TileWidth, TileHeight values
    tileWidth = int(0.8 * (float)boardRect.w / columns);
    tileHeight = int(0.8 * (float)boardRect.h / rows);
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
        tm tm {};

        pathTime.a = gameData.path().string();

        // now get timeString by removing the path to the folder + copy value
        //remove folder path string leaving just date + copy
        pathTimeString = pathTime.a.substr(gameDataDirPath.length()+1, std::string::npos);

        // ensure folder name is long enough to house the full time string
        if (pathTimeString.length() >= timeStringFormat.size() - 1) {
            pathTimeString.erase(timeStringFormat.size() -1, std::string::npos);

            // now turn into tm, >> is highlighted as an error yet works???
            std::istringstream ss(pathTimeString);
            ss >> std::get_time(&tm, timeFormat.c_str());

            // turn into time_t which can be stored then compared
            pathTime.b = mktime(&tm);

        } else {
            // default to time 0
            pathTime.b = 0;
        }

        pathTimes.push_back(pathTime);
    }

    // sort by time oldest -> newest
    std::sort(pathTimes.begin(), pathTimes.end(), [&](const AsymPair<std::string, time_t>& pathTimeA, const AsymPair<std::string, time_t>& pathTimeB){
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
        std::string pInfoStr = pInfo->color + "," + pInfo->name + "," + pInfo->gamepos.x + "," + std::to_string(pInfo->gamepos.y);
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
