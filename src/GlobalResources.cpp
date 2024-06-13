//
// Created by cew05 on 09/06/2024.
//

#include "src_headers/GlobalResources.h"

/*
 * ENUMS DECLARATION
 */

bool InitFonts() {
    auto fm = FontManager::GetInstance();

    fm->CreateNewFont("../Resources/Fonts/CF/TCFR.ttf", 100, Font::CONFESSION);

    return true;
}

bool InitTextures() {
    auto tm = TextureManager::GetInstance();

    // Temp vars
    std::string dirPath;
    int dirCount;

    /*
     * Construct Board Textures
     */

    // Produce texture info for all .png files in Resources/GameBoard directory
    dirPath = "../Resources/GameBoard";
    dirCount = 0;
    for (const auto& board : std::filesystem::directory_iterator(dirPath)) {
        // Ignore non-directory entries
        if (!board.is_directory()) continue;

        // Create new style info
        BOARD_STYLES[dirCount] = {board.path().filename().string(), (Texture)dirCount};

        // Create the styled Board Textures
        tm->NewTexture(board.path().string() + "/Board_Base.png", Texture(BOARD_BASE + BOARD_STYLE.second));
        tm->NewTexture(board.path().string() + "/Secondary_Base.png", Texture(BOARD_BASE_SECONDARY + BOARD_STYLE.second));
        tm->NewTexture(board.path().string() + "/White_Tile.png", Texture(WHITE_TILE + BOARD_STYLE.second));
        tm->NewTexture(board.path().string() + "/Black_Tile.png", Texture(BLACK_TILE + BOARD_STYLE.second));
        tm->NewTexture(board.path().string() + "/Promotion_Menu.png", Texture(PROMO_BASE + BOARD_STYLE.second));

        dirCount++;
    }

    // Create additional board textures
    tm->NewTexture(nullptr, BOARD_COMPILED);
    tm->NewTexture(nullptr, PROMO_BLACK_COMPILED);
    tm->NewTexture(nullptr, PROMO_WHITE_COMPILED);

    /*
     * Construct Menus Textures
     */

    // Produce texture info for the .png files in Resources/Menus


    /*
     * Construct Pieces Textures
     */

    // Produce texture info for all .png files in Resources/Pieces
    dirPath = "../Resources/Pieces";
    dirCount = 0;
    for (const auto& piece : std::filesystem::directory_iterator(dirPath)) {
        if (!piece.is_directory()) continue;
        if (piece.path().filename() == "Piece") continue;

        // Create style info
        PIECE_STYLES[dirCount] = {piece.path().filename().string(), (Texture)dirCount};

        // Create styled piece textures
        tm->NewTexture(piece.path().string() + "/King/King_White.png", Texture(WHITE_KING + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/King/King_Black.png", Texture(BLACK_KING + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Queen/Queen_White.png", Texture(WHITE_QUEEN + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Queen/Queen_Black.png", Texture(BLACK_QUEEN + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Rook/Rook_White.png", Texture(WHITE_ROOK + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Rook/Rook_Black.png", Texture(BLACK_ROOK + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Bishop/Bishop_White.png", Texture(WHITE_BISHOP + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Bishop/Bishop_Black.png", Texture(BLACK_BISHOP + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Knight/Knight_White.png", Texture(WHITE_KNIGHT + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Knight/Knight_Black.png", Texture(BLACK_KNIGHT + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Pawn/Pawn_White.png", Texture(WHITE_PAWN + PIECE_STYLE.second));
        tm->NewTexture(piece.path().string() + "/Pawn/Pawn_Black.png", Texture(BLACK_PAWN + PIECE_STYLE.second));

        dirCount++;
    }

    // Additional Piece textures
    tm->NewTexture(dirPath + "/Piece/Attacking_Piece.png", CAPTURE);
    tm->NewTexture(dirPath + "/Piece/Available_Space.png", MOVE);
    tm->NewTexture(dirPath + "/Piece/Selected_Target.png", SELECTED);

    return true;
}

/*
 * CONFIG FILES
 */

bool ConfigExists() {
    /*
     * Check if the config files exist. If not, attempt to create them. If config files cannot be made for whatever
     * reason, returns false
     */
    std::error_code ec;

    // required files stored in pairs of {name, path}
    Pair<std::string> reqFilesDir = {"RequiredFiles", "../RequiredFiles"};
    Pair<std::string> configInfoFile = {"ConfigInfo", reqFilesDir.b + "/ConfigInfo"};

    // ensure RequiredFiles directory exists
    if (!std::filesystem::exists(reqFilesDir.b)) {
        printf("%s folder not found. Creating.\n", reqFilesDir.a.c_str());
        if (!std::filesystem::create_directory(reqFilesDir.b, ec)) {
            LogError("Failed to create folder.", ec.message().c_str(), true);
            return false;
        }
    }

    // Ensure ConfigInfo file exists
    if (!std::filesystem::exists(configInfoFile.b)) {
        printf("%s file not found. Creating.\n", configInfoFile.a.c_str());

        std::ofstream configFile(configInfoFile.b);
        if (!configFile.good()) {
            configFile.close();
            LogError("Failed to create file.", "", true);
            return false;
        }
        configFile.close();
    }

    // no issues encountered, files + dir exist
    return true;
};

bool PieceLayoutsExists() {
    /*
     * Ensures that the piece layouts (standard games + base variations) exist. If not, then they are created.
     */

    // Standard Layout
    return true;
}

bool SetStyles() {
    BOARD_STYLE = BOARD_STYLES[0];
    PIECE_STYLE = PIECE_STYLES[0];

    return true;
}