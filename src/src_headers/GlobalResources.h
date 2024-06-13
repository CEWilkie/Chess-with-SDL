//
// Created by cew05 on 12/06/2024.
//

#ifndef CHESS_WITH_SDL_GLOBALRESOURCES_H
#define CHESS_WITH_SDL_GLOBALRESOURCES_H

#include <filesystem>
#include <fstream>
#include "GlobalSource.h"
#include "ResourceManagers.h"

/*
 * GLOBAL const vars used in setting enum values
 */

inline const int PIECE_STYLES_MAX = 5;
inline const int BOARD_STYLES_MAX = 5;

//...
//...
//...

/*
 * ENUMS for GLOBAL TextureManager
 * reserved values for styles / variants of textures included
 */

inline const int NUM_BOARD_TEXTURES = 5, NUM_PIECE_TEXTURES = 12;
enum Texture : int{
    // Board Textures
    BOARD_BASE = 0, BOARD_BASE_SECONDARY, WHITE_TILE, BLACK_TILE, PROMO_BASE,
    BOARD_COMPILED = NUM_BOARD_TEXTURES*BOARD_STYLES_MAX, PROMO_WHITE_COMPILED, PROMO_BLACK_COMPILED,


    // Piece textures
    WHITE_KING, BLACK_KING, WHITE_QUEEN, BLACK_QUEEN, WHITE_ROOK, BLACK_ROOK, WHITE_BISHOP, BLACK_BISHOP,
    WHITE_KNIGHT, BLACK_KNIGHT, WHITE_PAWN, BLACK_PAWN,

    // Movement textures
    MOVE = WHITE_KING + NUM_PIECE_TEXTURES*PIECE_STYLES_MAX, CAPTURE, SELECTED,
    //new group = ...

};

/*
 * Enums for GLOBAL FontManager
 */

enum class Font {CONFESSION};

/*
 * Global Vars to house the current Styles
 */

inline std::pair<std::string, Texture> PIECE_STYLES[PIECE_STYLES_MAX] {};
inline std::pair<std::string, Texture> PIECE_STYLE {};

inline std::pair<std::string, Texture> BOARD_STYLES[BOARD_STYLES_MAX] {};
inline std::pair<std::string, Texture> BOARD_STYLE {};

//...
//...
//...


/*
 * Initialiser Functions to create textures and fonts from Enums in ResourceManagers.
 */


bool InitFonts();
bool InitTextures();

/*
 * GLOBAL functions to detect and construct CONFIG files
 */

bool ConfigExists();

bool PieceLayoutsExists();

bool SetStyles();

#endif //CHESS_WITH_SDL_GLOBALRESOURCES_H
