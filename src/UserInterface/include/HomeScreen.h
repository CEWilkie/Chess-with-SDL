//
// Created by cew05 on 16/06/2024.
//

#ifndef CHESS_WITH_SDL_HOMESCREEN_H
#define CHESS_WITH_SDL_HOMESCREEN_H

#include "AppScreen.h"

/*
 * HomeScreen
 */

class HomeScreen : public AppScreen {
    private:
        enum MenuID : int {
            MAIN_MENU
        };

        enum buttonID : int {
            PLAY_VS_AI, PLAY_NETWORK, REVIEW_GAMES,
        };

    public:
        HomeScreen();

        void HandleEvents() override;
        void CheckButtons() override;
};

#endif //CHESS_WITH_SDL_HOMESCREEN_H
