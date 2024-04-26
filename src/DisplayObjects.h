//
// Created by cew05 on 24/04/2024.
//

#ifndef CHESS_WITH_SDL_DISPLAYOBJECTS_H
#define CHESS_WITH_SDL_DISPLAYOBJECTS_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "GlobalSource.h"

class DisplayObject {
    private:
        SDL_Texture* texture {};

    public:
        int CreateTexture(const std::string& imgPath);
};


#endif //CHESS_WITH_SDL_DISPLAYOBJECTS_H
