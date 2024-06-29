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

struct Obj_Window {
    SDL_Window* window {};
    SDL_Renderer* renderer {};
    SDL_Rect currentRect {};
    SDL_Rect minRect {};
    SDL_Texture* background {};
} ;
inline Obj_Window window;

struct FrameTick {
    Uint64 currTick;
    Uint64 lastTick;
    Uint64 tickChange;
};
inline FrameTick frameTick;

class Mouse {
    private:
        bool nextActive = false;
        bool active = false;
        bool prevactive = false;
        bool heldactive = false;
        std::pair<int, int> position {};

    public:
        void MouseDown(bool _isDown) {
            nextActive = _isDown;
        }

        void Update() {
            // Update click values
            prevactive = active;
            active = nextActive;
            heldactive = (prevactive && active);

            UpdatePosition();
        }

        void PrintStates() {
            printf("_ACTIVE : %s\n_PREVACTIVE : %s\n_HELDACTIVE : %s\n",
                   (active ? "y" : "n"), (prevactive ? "y" : "n"), (heldactive ? "y" : "n") );
        }

        void UpdatePosition() {
            // Update position
            SDL_GetMouseState(&position.first, &position.second);
        }

        std::pair<int, int> GetMousePosition() { return position; }

        bool InRadius(std::pair<int, int> pos, int radius) {
            UpdatePosition();
            return (std::pow(position.first - pos.first, 2) + std::pow(position.second - pos.second, 2) <= std::pow(radius, 2));
        }

        bool InRect(SDL_Rect rect) {
            UpdatePosition();
            return abs(position.first - (rect.x + rect.w / 2)) <= rect.w / 2 && abs(position.second - (rect.y + rect.h / 2)) <= rect.h / 2;
        }

        bool UnheldClick(SDL_Rect rect) {
            UpdatePosition();
            return active && !heldactive && InRect(rect);
        }

        bool ClickOnRelease(SDL_Rect rect) {
            UpdatePosition();
            return !active && prevactive && InRect(rect);
        }

        bool IsHeldActive() const {
            return active;
        }

        bool IsUnheldActive() const {
            return active && !prevactive;
        }
};
inline Mouse mouse;

#endif //CHESS_WITH_SDL_GLOBALSOURCE_H
