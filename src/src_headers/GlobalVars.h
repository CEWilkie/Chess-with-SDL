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
 * This h file is to obtain and then store global vars for settings / config info like piece styles or text formatting
 * info. Global vars from this h file will be capitalised to indicate origin/global status.
 */

inline std::string PIECE_STYLE = "Temp";
//... other vars here

bool ConfigExistsAIsufbasf() {
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
}

#endif //CHESS_WITH_SDL_GLOBALVARS_H
