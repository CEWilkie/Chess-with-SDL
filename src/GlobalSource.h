//
// Created by cew05 on 19/04/2024.
//

#ifndef CHESS_WITH_SDL_GLOBALSOURCE_H
#define CHESS_WITH_SDL_GLOBALSOURCE_H

#include <iostream>
#include <SDL.h>

template<class T>
void LogError(const std::string& errorMsgDecorative = "", const T& errorMsg = "", bool fatal = false) {
    printf("New Error Raised!\nError Desc: %s\nError Proper: %s\nError Fatal?  %d\n",
           errorMsgDecorative.c_str(), errorMsg, fatal);
}

struct Position{
        int x;
        int y;
    };

struct Obj_Window {
    SDL_Window* window {};
    SDL_Renderer* renderer {};
    SDL_Rect currentRect {};
    SDL_Rect minRect {};
} ;

inline Obj_Window window;

#endif //CHESS_WITH_SDL_GLOBALSOURCE_H
