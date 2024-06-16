//
// Created by cew05 on 16/06/2024.
//

#include "src_headers/MainMenu.h"

MainMenu::MainMenu(std::pair<int, int> position, std::pair<int, int> size, const std::string &title) : Menu(position, size, title) {
    // Construct MainMenu Buttons
    Button* button;

    // Define n buttons
    const int b = 4;
    ButtonID id[b] {ButtonID::AGAINST_BOT, ButtonID::AGAINST_LOCAL, ButtonID::AGAINST_NETWORK,
                   ButtonID::REPLAY};
    std::string labels[b] {"Play against AI", "Play local multiplayer", "Play local Network", "Replay saved games"};
    for (int bID = 0; bID < b; bID++){
        std::pair<int, int> buttonSize = {menuRect.w/2, menuRect.h/16};
        std::pair<int, int> buttonPos = {menuRect.x + menuRect.w/4, menuRect.y + menuRect.h/2 + bID*buttonSize.second};

        button = new Button(buttonPos, buttonSize, labels[bID]);
        buttonManager->NewResource(button, id[bID]);
    }
}
