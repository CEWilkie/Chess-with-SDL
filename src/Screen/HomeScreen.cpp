//
// Created by cew05 on 16/06/2024.
//

#include "include/HomeScreen.h"

/*
 * HomeScreen definitions
 */

HomeScreen::HomeScreen() : AppScreen() {
    // Temp vars
    Menu* menu;
    Button* button;

    // Create menu
    menu = new Menu({0, 0}, {window.currentRect.w, window.currentRect.h}, "Welcome to ChesSDL!");
    menu->CanClose(false);

    // Add buttons to main menu
    auto mmButtonMap = menu->AccessButtonManager();
    button = new Button({screenRect.w/4, screenRect.h/4},
                        {screenRect.w/2, screenRect.h/16},
                        "Play against AI");
    mmButtonMap->NewResource(button, PLAY_VS_AI);

    button = new Button({screenRect.w/4, 2*screenRect.h/4},
                        {screenRect.w/2, screenRect.h/16},
                        "Play on Local Network");
    mmButtonMap->NewResource(button, PLAY_NETWORK);

    button = new Button({screenRect.w/4, 3*screenRect.h/4},
                        {screenRect.w/2, screenRect.h/16},
                        "Review past games");
    mmButtonMap->NewResource(button, REVIEW_GAMES);

    // add menu to manager
    menuManager->NewResource(menu, MenuID::MAIN_MENU);

    // Add non-menu Button resources
    // ...
}



void HomeScreen::HandleEvents() {
    AppScreen::HandleEvents();

    //... Rest of events
}

void HomeScreen::CheckButtons() {
    bool ignoreInput;
    stateManager->FetchResource(ignoreInput, NO_INPUT);
    if (ignoreInput) return;

    Menu* menu;
    Button* button;
    GenericManager<Button*>* menuButtonManager;

    // Fetch buttonManager from menu
    menuManager->FetchResource(menu, MAIN_MENU);
    menuButtonManager = menu->AccessButtonManager();

    // PLAY_VS_AI clicked
    menuButtonManager->FetchResource(button, PLAY_VS_AI);
    if (button->IsClicked()) {
        stateManager->UpdateResource(true, NO_INPUT);
    }

    //... Rest of events
}


