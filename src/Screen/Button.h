//
// Created by cew05 on 19/06/2024.
//

#ifndef CHESS_WITH_SDL_BUTTON_H
#define CHESS_WITH_SDL_BUTTON_H

#include "../src_headers/GlobalResources.h"
#include "SDL_ttf.h"

enum ButtonTexture : int {
    // enums to specify the base texture showing on user input
    NORMAL, HOVER, CLICKED,

    // enums to specify the icon to be used in the button texture
    ICON_NONE, ICON_MENU, ICON_RESIGN, ICON_OFFERDRAW, ICON_CLOSE,
};

class Button {
    private:
        // ButtonID RectID
        SDL_Rect buttonRect {};

        // ButtonID display
        std::string label;
        ButtonTexture iconID;
        float textBorder = 0.1;
        bool lockRatio = true;
        bool lockSize = false;
        bool loaded = false;

        // ButtonID functionality
        int edgeRadius = 0;
        bool clickStarted = false;
        bool clicked = false;

        // Local texture manager
        TextureManager *buttonTextures = new TextureManager(window.renderer);

    public:
        Button(std::pair<int, int> _position, std::pair<int, int> _size, const std::string &_label);
        Button(std::pair<int, int> _position, std::pair<int, int> _size, ButtonTexture _iconID);

        // Display
        bool CreateTextures();
        bool LoadButton(bool _load);
        void LockSize(bool _lock);
        void LockRatio(bool _lock);
        bool Display();

        // Update rect
        SDL_Rect FetchButtonRect();
        void UpdateSize(std::pair<int, int> _size);
        void UpdatePosition(std::pair<int, int> _position);

        // Functionality
        bool MouseOverButton();
        void UpdateClickedStatus();
        [[nodiscard]] bool IsClicked() const;
};


#endif //CHESS_WITH_SDL_BUTTON_H
