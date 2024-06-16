//
// Created by cew05 on 16/06/2024.
//

#ifndef CHESS_WITH_SDL_MAINMENU_H
#define CHESS_WITH_SDL_MAINMENU_H

#include "AppScreen.h"

class MainMenu : public Menu{
    private:
        // Local ResourceManagers
        enum class ButtonID;
        ResourceManager<ButtonID, Button*>* buttonManager = new ResourceManager<ButtonID, Button*>;

    public:
        MainMenu(std::pair<int, int> position, std::pair<int, int> size, const std::string &title);

        // buttonFunctions
};

enum class MainMenu::ButtonID {
        AGAINST_BOT, AGAINST_LOCAL, AGAINST_NETWORK, REPLAY,
};

/*
 * Submenus of mainMenu
 */



#endif //CHESS_WITH_SDL_MAINMENU_H
