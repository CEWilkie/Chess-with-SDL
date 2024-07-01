//
// Created by cew05 on 19/06/2024.
//

#ifndef CHESS_WITH_SDL_MENU_H
#define CHESS_WITH_SDL_MENU_H

#include "../../src_headers/GlobalResources.h"
#include "Button.h"

class Menu {
    private:
        enum texture : int {
            BACKGROUND
        };

    protected:
        // Positioning vars
        SDL_Rect menuRect {};

        // Local ResourceManagers
        GenericManager<Button*>* buttonManager = new GenericManager<Button*>;
        TextureManager* textureManager = new TextureManager(window.renderer);

        // Functionality
        Button* closeButton = nullptr;
        bool active = true;
        bool canClose = true;

        // DisplayToggle vars
        std::string menuTitle;
        bool showBackground = true;
        bool lockSize = false;
        bool lockRatio = false;

    public:
        Menu(std::pair<int, int> _position, std::pair<int, int> _size, const std::string& _title);

        // Display
        bool CreateTextures();
        bool LoadMenu(bool _load);
        bool Display();
        void CanClose(bool _closable);
        void LockSize(bool _lock);
        void LockRatio(bool _lock);

        // update rect
        SDL_Rect FetchMenuRect();
        void UpdatePosition(std::pair<int, int> _position);
        void UpdateSize(std::pair<int, int> _size);

        // Button functionality
        GenericManager<Button*>* AccessButtonManager();
        void UpdateButtonStates();
        virtual void CheckButtons();

        // callable functions for buttons
        void CloseMenu();
};


#endif //CHESS_WITH_SDL_MENU_H
