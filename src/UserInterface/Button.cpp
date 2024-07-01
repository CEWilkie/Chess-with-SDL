//
// Created by cew05 on 19/06/2024.
//

#include "include/Button.h"

Button::Button(std::pair<int, int> _position, std::pair<int, int> _size, const std::string &_label) {
    // Set local vars
    label = _label;
    iconID = ButtonTexture::ICON_NONE;
    buttonRect = {_position.first, _position.second, _size.first, _size.second};

    // Create texture resources
    buttonTextures->NewTexture(nullptr, ButtonTexture::NORMAL);
    buttonTextures->NewTexture(nullptr, ButtonTexture::HOVER);
    buttonTextures->NewTexture(nullptr, ButtonTexture::CLICKED);
}

Button::Button(std::pair<int, int> _position, std::pair<int, int> _size, int _iconID) :
        Button(_position, _size, "") {
    // set local vars
    iconID = _iconID;
}

Button::Button(std::pair<int, int> _position, std::pair<int, int> _size, int _iconID, bool _externIcon) :
        Button(_position, _size, ""){
    usingButtonIcon = false;
    iconID = _iconID;
}

bool Button::CreateTextures() {
    // temp vars
    tm->OpenTexture(BUTTON_SHEET);
    SDL_Texture* buttonSheet = tm->AccessTexture(BUTTON_SHEET);
    TTF_Font* font = TTF_OpenFont("../Resources/Fonts/CF/TCFR.ttf", 100);

    std::pair<int, int> textureGrid = tm->AccessInfo(BUTTON_SHEET)->grid;
    std::pair<int, int> tgSize;
    SDL_Rect srcRect, destRect;

    ButtonTexture ids[] = {ButtonTexture::NORMAL, ButtonTexture::HOVER, ButtonTexture::CLICKED};
    for (int t = 0; t < 3; t++) {
        // Create texture to draw to
        SDL_Texture* buttonTexture = SDL_CreateTexture(window.renderer,
                                                       SDL_PIXELFORMAT_RGBA8888,
                                                       SDL_TEXTUREACCESS_TARGET,
                                                       buttonRect.w, buttonRect.h);
        if (buttonTexture == nullptr) {
            LogError("Failed to create button texture", SDL_GetError());
            return false;
        }

        // Set renderTarget
        if (SDL_SetRenderTarget(window.renderer, buttonTexture) != 0) {
            LogError("Failed to set render target", SDL_GetError());
            return false;
        }

        // Set blending modes
        SDL_SetRenderDrawBlendMode(window.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(buttonTexture, SDL_BLENDMODE_BLEND);

        // Get textureSheet dimensions to then get dimensions of each texture in the grid
        int sheetWidth, sheetHeight;
        SDL_QueryTexture(buttonSheet, nullptr, nullptr, &sheetWidth, &sheetHeight);
        tgSize.first = sheetWidth / textureGrid.first;
        tgSize.second = sheetHeight / textureGrid.second;

        // Ratio to store scale factor from srcHeight to destHeight
        // the scale factor is only needed when w > h, else a circle is formed using only the ends.
        double sf = (double) buttonRect.h / tgSize.second;;

        // Draw left side of the button
        srcRect = {0, t*tgSize.second, tgSize.first, tgSize.second};
        int drWidth = (buttonRect.w > buttonRect.h) ? int(tgSize.first * sf) : buttonRect.w;
        destRect = {0, 0, drWidth, buttonRect.h};
        if (SDL_RenderCopy(window.renderer, buttonSheet, &srcRect, &destRect)) {
            LogError("Failed to draw texture to buttonNormal", SDL_GetError());
        }

        // if width > height, then get the difference as the width for a filler rect between the two ends of the button
        if (buttonRect.w > buttonRect.h) {
            srcRect.x += srcRect.w;
            destRect = {drWidth / 2, 0, buttonRect.w - drWidth, buttonRect.h};
            if (SDL_RenderCopy(window.renderer, buttonSheet, &srcRect, &destRect)) {
                LogError("Failed to draw texture to buttonNormal", SDL_GetError());
            }
        }

        // if the width and height are the same, then button is a circle, use diff right end texture
        (buttonRect.w <= buttonRect.h) ? srcRect.x = 3 * srcRect.w : srcRect.x = 2 * srcRect.w;
        destRect = {buttonRect.w - drWidth, 0, drWidth, buttonRect.h};
        if (SDL_RenderCopy(window.renderer, buttonSheet, &srcRect, &destRect)) {
            LogError("Failed to draw texture to buttonNormal", SDL_GetError());
        }

        // If the label is not empty, draw the label
        if (!label.empty()) {
            SDL_Surface *textSurface = TTF_RenderText_Blended(font, label.c_str(), {255, 255, 255, 255});
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(window.renderer, textSurface);

            // Resize texture
            int labelW, labelH;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &labelW, &labelH);
            sf = (double) labelH / ((double)buttonRect.h * (1 - 2 * textBorder));
            labelH = int((double)buttonRect.h * (1 - 2 * textBorder));
            labelW = int((double) labelW / sf);

            // Centralise label in button
            destRect = {buttonRect.w / 2 - labelW / 2, int(textBorder * (double)buttonRect.h), labelW, labelH};
            SDL_RenderCopy(window.renderer, textTexture, nullptr, &destRect);
        }

        // Check for drawing an icon, and if the icon is part of the global texture manager
        if (iconID != ButtonTexture::ICON_NONE) {
            if (!usingButtonIcon) {
                // using global texture for icon
                SDL_Texture* iconTexture = tm->AccessTexture(iconID);

                // Centralise the icon
                destRect = {buttonRect.w/2 - buttonRect.h / 2, 0, buttonRect.h, buttonRect.h};
                if (SDL_RenderCopy(window.renderer, iconTexture, nullptr, &destRect) != 0) {
                    LogError("Failed to draw NON-BUTTON-ICON", SDL_GetError());
                }
            }
            else {
                // using button icon for texture
                srcRect.x = 4*srcRect.w;
                srcRect.y = (((int)iconID - (int)ButtonTexture::ICON_NONE) - 1) * srcRect.h;

                // Centralise the icon
                destRect = {buttonRect.w/2 - buttonRect.h / 2, 0, buttonRect.h, buttonRect.h};
                if (SDL_RenderCopy(window.renderer, buttonSheet, &srcRect, &destRect) != 0) {
                    LogError("Failed to draw BUTTON ICON", SDL_GetError());
                }
            }
        }

        buttonTextures->UpdateTexture(buttonTexture, ids[t]);

        // store edgeRadius
        edgeRadius = drWidth / 2;
    }

//    SDL_Surface* surface = SDL_CreateRGBSurface(0, buttonRect.w, buttonRect.h, 32, 0, 0, 0, 0);
//    SDL_RenderReadPixels(window.renderer, nullptr, surface->format->format, surface->pixels, surface->pitch);
//    IMG_SavePNG(surface, "testButton.png");
//    SDL_FreeSurface(surface);

    // reset renderTarget
    SDL_SetRenderTarget(window.renderer, nullptr);

    // Close textures
    tm->CloseTexture(BUTTON_SHEET);
    TTF_CloseFont(font);

    return true;
}

bool Button::LoadButton(bool _load) {
    //... non functional

    return true;
}

void Button::LockSize(bool _lock) {
    lockSize = _lock;
}

void Button::LockRatio(bool _lock) {
    lockRatio = _lock;
}

bool Button::Display() {
    // Get texture id
    ButtonTexture id = (MouseOverButton()) ? ButtonTexture::HOVER : ButtonTexture::NORMAL;
    if (clickStarted) id = ButtonTexture::CLICKED;

    SDL_Texture* buttonTexture = buttonTextures->AccessTexture(id);
    if (buttonTexture == nullptr) {
        LogError(TextureManager::GetIssueString(buttonTextures->GetIssue()), "");
        return false;
    }
    if (SDL_RenderCopy(window.renderer, buttonTexture, nullptr, &buttonRect) != 0) {
        LogError("Failed to display Button", SDL_GetError(), false);
        return false;
    }

    return true;
}

SDL_Rect Button::FetchButtonRect() {
    return buttonRect;
}

void Button::UpdateSize(std::pair<int, int> _size) {
    if (lockSize) return;

    double sf = (double)_size.first / buttonRect.w ;
    buttonRect.w = _size.first;
    buttonRect.h = (lockRatio ? int(buttonRect.h * sf) : _size.second);
}

void Button::UpdatePosition(std::pair<int, int> _position) {
    buttonRect.x = _position.first;
    buttonRect.y = _position.second;
}


bool Button::MouseOverButton() {
    bool hover = false;

    // Is the mouse hovering over the left edge of the button
    if (mouse.InRadius({buttonRect.x + edgeRadius, buttonRect.y + edgeRadius}, edgeRadius))
        hover = true;

    // If the width is greater than height, is the mouse hovering over the middle rect
    if (buttonRect.w > buttonRect.h) {
        if (mouse.InRect({buttonRect.x+edgeRadius, buttonRect.y, buttonRect.w - 2*edgeRadius, buttonRect.h}))
            hover = true;
    }

    // Is the mouse hovering over the right edge of the button
    if (mouse.InRadius({buttonRect.x + buttonRect.w - edgeRadius, buttonRect.y + edgeRadius}, edgeRadius))
        hover = true;

    // if width and height are the same, circular button
    if (buttonRect.w == buttonRect.h) {
        if (mouse.InRadius({buttonRect.x + buttonRect.w / 2, buttonRect.y + buttonRect.h/2}, buttonRect.w/2)) {
            hover = true;
        }
    }

    return hover;
}

void Button::UpdateClickedStatus() {
    // ButtonID is only considered clicked when the user releases mousedown over the button, after having started their
    // initial mouseDown input over the button.
    clicked = false;

    // mousedown over the button
    if (MouseOverButton() && mouse.IsUnheldActive()) {
        clickStarted = true;
    }

    // mousedown released, over the button
    if (clickStarted && mouse.ClickOnRelease(buttonRect)) {
        clickStarted = false;
        clicked = true;
    }

    // mousedown released, not over button
    if (clickStarted && !mouse.IsHeldActive()) clickStarted = false;
}

bool Button::IsClicked() const {
    return clicked;
}