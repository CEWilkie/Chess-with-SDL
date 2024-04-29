//
// Created by cew05 on 19/04/2024.
//

#ifndef CHESS_WITH_SDL_GLOBALSOURCE_H
#define CHESS_WITH_SDL_GLOBALSOURCE_H

#include <iostream>
#include <cmath>
#include <SDL.h>

template<class T>
void LogError(const std::string& errorMsgDecorative = "", const T& errorMsg = "", bool fatal = false) {
    printf("New Error Raised!\nError Desc: %s\nError Proper: %s\nError Fatal?  %d\n",
           errorMsgDecorative.c_str(), errorMsg, fatal);
}

template<class A, class B>
struct Position{
        A x;
        B y;
    };

struct Obj_Window {
    SDL_Window* window {};
    SDL_Renderer* renderer {};
    SDL_Rect currentRect {};
    SDL_Rect minRect {};
    SDL_Texture* background {};
} ;
inline Obj_Window window;


struct Mouse_Input {
    bool active = false;
    bool prevactive = false;
    bool heldactive = false;
    bool occupied = false;
    Position<int, int> position {};

    void UpdateState() {
        // Update position
        SDL_GetMouseState(&position.x, &position.y);

        // Update holdvalue
        heldactive = (prevactive && active);
    }

    bool InRadius(Position<int, int> pos, int radius) const {
        // Return result
        return (std::pow(position.x - pos.x, 2) + std::pow(position.y - pos.y, 2) == std::pow(radius, 2));
    }

    bool InRect(SDL_Rect rect) const {
        // return result
        return abs(position.x - (rect.x + rect.w/2)) <= rect.w/2 && abs(position.y - (rect.y + rect.h/2)) <= rect.h/2;
    }
};
inline Mouse_Input mouseInput;

#endif //CHESS_WITH_SDL_GLOBALSOURCE_H
