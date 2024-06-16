//
// Created by cew05 on 16/06/2024.
//

#ifndef CHESS_WITH_SDL_APPSCREEN_H
#define CHESS_WITH_SDL_APPSCREEN_H

#include "GlobalSource.h"
#include "ResourceManagers.h"

/*
 * ButtonTextureID sets const values to identifiers. These identifiers are used to denote the textures to be taken from
 * the button texture sheet.
 */

enum class ButtonTextureID {
    // enums to specify the base texture showing on user input
    NORMAL, HOVER, CLICKED,

    // enums to specify the icon to be used in the button texture
    ICON_NONE, ICON_MENU, ICON_RESIGN, ICON_OFFERDRAW, ICON_CLOSE,
};

/*
 * Generic Button Class which may house a label or icon.
 */

class Button {
    private:
        // Button Rect
        SDL_Rect buttonRect {};

        // Button display
        std::string label;
        ButtonTextureID iconID;
        float textBorder = 0.1;

        // Button functionality
        int edgeRadius = 0;
        bool clickStarted = false;
        bool clicked = false;

        // Local texture manager
        ResourceManager<ButtonTextureID, SDL_Texture*>* buttonTextures = new ResourceManager<ButtonTextureID, SDL_Texture*>;

    public:
        Button(std::pair<int, int> _position, std::pair<int, int> _size, const std::string& _label);
        Button(std::pair<int, int> _position, std::pair<int, int> _size, ButtonTextureID _iconID);
        virtual bool CreateTextures();

        bool MouseOverButton();
        bool Display();
        bool ClickedStatus();
};

/*
 * Generic Menu Class. Provides the basics for a menu, such as a visible object and a close button.
 * Menus are further specialised within Subclasses inheriting the menu. These subclasses define the buttons within a
 * menu and provide the functions the buttons execute.
 */

class Menu {
    private:

    protected:
        // Positioning vars
        SDL_Rect menuRect {};

        // Local ResourceManagers
        enum class ButtonID;
        ResourceManager<ButtonID, Button*>* buttonManager = new ResourceManager<ButtonID, Button*>;

        // Display vars
        std::string menuTitle;
        SDL_Texture* background = nullptr;
        bool showBackground = true;
        bool canDisplay = true;

        // User Manipulation states
        Button* closeButton;
        bool canClose = true;
        bool canCollapse = false;
        bool canDrag = false;

    public:
        Menu(std::pair<int, int> _position, std::pair<int, int> _size, const std::string& _title);
        bool CreateTextures();
        bool Display();
        SDL_Rect FetchMenuRect();
        bool UpdatePosition(std::pair<int, int> _position);
        bool UpdateSize(std::pair<int, int> _size);
        bool CloseClicked();
};

/*
 * Define the base screen from which all individual screens of the program will be constructed (home menu, gameboard,
 * settings, selecting opponent ...).
 */

class AppScreen {
    protected:


    public:
        AppScreen();


        bool HandleEvents();

};


#endif //CHESS_WITH_SDL_APPSCREEN_H
