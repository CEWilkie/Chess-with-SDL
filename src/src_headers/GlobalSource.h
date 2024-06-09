//
// Created by cew05 on 19/04/2024.
//

#ifndef CHESS_WITH_SDL_GLOBALSOURCE_H
#define CHESS_WITH_SDL_GLOBALSOURCE_H

#include <iostream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <SDL.h>

/*
 * Main Definitions
 */

template<class T>
void LogError(const std::string& errorMsgDecorative = "", const T& errorMsg = "", bool fatal = false) {
    printf("New Error Raised!\nError Desc: %s\nError Proper: %s\nError Fatal?  %d\n",
           errorMsgDecorative.c_str(), errorMsg, fatal);
}

template<class A>
struct Pair{
        A a;
        A b;
    };

template<class A, class B>
struct AsymPair{
            A a;
            B b;
        };

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


class Mouse {
    private:
        bool active = false;
        bool prevactive = false;
        bool heldactive = false;
        bool clickLock = false;
        Position<int, int> position {};

    public:
        void UpdateState(bool mouseDown) {
            // Update position
            SDL_GetMouseState(&position.x, &position.y);

            // prevent mouseInputs
            if (clickLock) return;

            // Update click values
            prevactive = active;
            active = mouseDown;
            heldactive = (prevactive && active);
        }

        Position<int, int> GetMousePosition() { return position; }

        void LockMouseInput(bool lock) { clickLock = lock; }

        bool InRadius(Position<int, int> pos, int radius) const {
            // Return result
            return (std::pow(position.x - pos.x, 2) + std::pow(position.y - pos.y, 2) == std::pow(radius, 2));
        }

        bool InRect(SDL_Rect rect) const {
            // return result
            return abs(position.x - (rect.x + rect.w/2)) <= rect.w/2 && abs(position.y - (rect.y + rect.h/2)) <= rect.h/2;
        }

        bool UnheldClick(SDL_Rect rect) const {
            return active && !heldactive && InRect(rect);
        }

        bool HeldClick(SDL_Rect rect) const {
            return active && heldactive && InRect(rect);
        }
};
inline Mouse mouse;

#endif //CHESS_WITH_SDL_GLOBALSOURCE_H
