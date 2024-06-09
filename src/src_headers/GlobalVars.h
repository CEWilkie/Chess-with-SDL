//
// Created by cew05 on 08/06/2024.
//

#ifndef CHESS_WITH_SDL_GLOBALVARS_H
#define CHESS_WITH_SDL_GLOBALVARS_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include "GlobalSource.h"

/*
 * Temp defs
 */

/*
 * This h file is to obtain and then store global vars for settings / config info like piece styles or text formatting
 * info. Global vars from this h file will be capitalised to indicate origin/global status.
 */

inline std::string PIECE_STYLE = "Temp";
//... other vars here

bool ConfigExists();

#endif //CHESS_WITH_SDL_GLOBALVARS_H
