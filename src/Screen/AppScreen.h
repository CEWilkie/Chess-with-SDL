//
// Created by cew05 on 16/06/2024.
//

#ifndef CHESS_WITH_SDL_APPSCREEN_H
#define CHESS_WITH_SDL_APPSCREEN_H

#include "SDL_ttf.h"

#include "../src_headers/GlobalSource.h"
#include "Menu.h"

/*
 * Define the base screen from which all individual screens of the program will be constructed (home menu, gameboard,
 * settings, selecting opponent ...).
 */

class AppScreen {
    protected:
        // Display vars
        SDL_Rect screenRect {};
        enum texture : int {
            BACKGROUND
        };

        // Button Functionality enums
        enum buttonID : int;

        // Local managers
        GenericManager<Menu*>* menuManager = new GenericManager<Menu*>;
        GenericManager<Button*>* buttonManager = new GenericManager<Button*>;
        TextureManager* textureManager = new TextureManager(window.renderer);

        GenericManager<bool>* stateManager = new GenericManager<bool>;

    public:
        AppScreen();

        // Display screen
        bool CreateTextures();
        bool LoadScreen();
        bool Display();

        // respositioning
        void ResizeScreen();

        // Event Handling
        virtual void HandleEvents();
        void UpdateButtonStates();
        virtual void CheckButtons();

        // Fetch states
        enum ScreenState : int {
            SCREEN_CLOSED,
        };
        [[nodiscard]] bool FetchScreenState(int _stateID);
};

#endif //CHESS_WITH_SDL_APPSCREEN_H
