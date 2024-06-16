//
// Created by cew05 on 16/06/2024.
//

#include "src_headers/AppScreen.h"

/*
 * Button class definitions
 */

Button::Button(std::pair<int, int> _position, std::pair<int, int> _size, const std::string &_label) {
    // Set local vars
    label = _label;
    iconID = ButtonTextureID::ICON_NONE;
    buttonRect = {_position.first, _position.second, _size.first, _size.second};

    // Create texture resources
    buttonTextures->NewResource(nullptr, ButtonTextureID::NORMAL);
    buttonTextures->NewResource(nullptr, ButtonTextureID::HOVER);
    buttonTextures->NewResource(nullptr, ButtonTextureID::CLICKED);
}

Button::Button(std::pair<int, int> _position, std::pair<int, int> _size, ButtonTextureID _iconID) {
    // set local vars
    iconID = _iconID;
    buttonRect = {_position.first, _position.second, _size.first, _size.second};

    // Create texture resources
    buttonTextures->NewResource(nullptr, ButtonTextureID::NORMAL);
    buttonTextures->NewResource(nullptr, ButtonTextureID::HOVER);
    buttonTextures->NewResource(nullptr, ButtonTextureID::CLICKED);
}

bool Button::CreateTextures() {
    // temp vars
    SDL_Texture* buttonSheet = tm->OpenTexture(BUTTON_SHEET);
    TTF_Font* font = fm->OpenExistingFont(Font::CONFESSION);

    std::pair<int, int> textureGrid = tm->FetchTextureInfo(BUTTON_SHEET).grid;
    std::pair<int, int> tgSize;
    SDL_Rect srcRect, destRect;

    for (int t = 0; t < 3; t++) {
        // Create texture to draw to
        SDL_Texture* buttonTexture = SDL_CreateTexture(window.renderer,
                                                       SDL_PIXELFORMAT_RGBA8888,
                                                       SDL_TEXTUREACCESS_TARGET,
                                                       buttonRect.w, buttonRect.h);

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
            sf = (double) labelH / (buttonRect.h * (1 - 2 * textBorder));
            labelH = int(buttonRect.h * (1 - 2 * textBorder));
            labelW = int((double) labelW / sf);

            // Centralise label in button
            destRect = {buttonRect.w / 2 - labelW / 2, int(textBorder * buttonRect.h), labelW, labelH};
            SDL_RenderCopy(window.renderer, textTexture, nullptr, &destRect);
        }

        // since label is empty, check that an icon can be loaded to draw
        if (iconID != ButtonTextureID::ICON_NONE) {
            srcRect.x = 4*srcRect.w;
            srcRect.y = (((int)iconID - (int)ButtonTextureID::ICON_NONE) - 1) * srcRect.h;

            // Centralise the icon
            destRect = {buttonRect.w/2 - buttonRect.h / 2, 0, buttonRect.h, buttonRect.h};
            SDL_RenderCopy(window.renderer, buttonSheet, &srcRect, &destRect);
        }

        ButtonTextureID ids[] = {ButtonTextureID::NORMAL, ButtonTextureID::HOVER, ButtonTextureID::CLICKED};
        buttonTextures->UpdateExistingResource(buttonTexture, ids[t]);

        // store edgeRadius
        edgeRadius = drWidth / 2;
    }

    // reset renderTarget
    SDL_SetRenderTarget(window.renderer, nullptr);

    // Close textures
    tm->CloseTexture(BUTTON_SHEET);
    fm->CloseFont(Font::CONFESSION);

    return true;
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

    return hover;
}

bool Button::Display() {
    // Get position of button on screen


    // Get texture id
    ButtonTextureID id = (MouseOverButton()) ? ButtonTextureID::HOVER : ButtonTextureID::NORMAL;
    if (clickStarted) id = ButtonTextureID::CLICKED;

    SDL_Texture* buttonTexture = buttonTextures->FetchResource(id);

    if (SDL_RenderCopy(window.renderer, buttonTexture, nullptr, &buttonRect) != 0) {
        LogError("Failed to display Button", SDL_GetError(), false);
        return false;
    }

    return true;
}

bool Button::ClickedStatus() {
    // Button is only considered clicked when the user releases mousedown over the button, after having started their
    // initial mouseDown input over the button.

    // mousedown over the button
    if (MouseOverButton() && mouse.IsUnheldActive()) {
        clickStarted = true;
    }

    // mousedown released, over the button
    if (clickStarted && mouse.ClickOnRelease(buttonRect)) {
        clickStarted = false;
        return true;
    }

    // mousedown released, not over button
    if (clickStarted && !mouse.IsHeldActive()) clickStarted = false;

    return false;
}




/*
 * Menu Definitions
 */

Menu::Menu(std::pair<int, int> _position, std::pair<int, int> _size, const std::string& _title) {
    menuTitle = _title;

    // Set rect dimensions and ensure min size
    menuRect = {_position.first, _position.second, _size.first, _size.second};
    menuRect.w = std::max(75, menuRect.w);
    menuRect.h = std::max(75, menuRect.h);

    // Construct Close Icon
    closeButton = new Button({menuRect.x + menuRect.w - 40, menuRect.y + 10}, {30, 30}, ButtonTextureID::ICON_CLOSE);

    // Construct button widgets
    // ... this would be done in the subclasses
}

bool Menu::CreateTextures() {
    fm->OpenExistingFont(Font::CONFESSION);

    // Open texture sheet
    SDL_Texture* menuSheet = tm->OpenTexture(MENU_SHEET);
    std::pair<int, int> menuGrid = tm->FetchTextureInfo(MENU_SHEET).grid;
    SDL_Rect menuSheetTile;

    // Get size of tile in menuSheet
    int w, h;
    SDL_QueryTexture(menuSheet, nullptr, nullptr, &w, &h);
    menuSheetTile = {0, 0, w / menuGrid.first, h / menuGrid.second};

    // Create texture to draw to
    // Create texture to draw to
    background = SDL_CreateTexture(window.renderer,
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET,
                                   menuRect.w, menuRect.h);

    // Set renderTarget
    if (SDL_SetRenderTarget(window.renderer, background) != 0) {
        LogError("Failed to set render target", SDL_GetError());
        return false;
    }

    // Set blending modes
    SDL_SetRenderDrawBlendMode(window.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);

    // Determine the scale factor
    double sf = (menuRect.w > menuRect.h) ? menuRect.h/2 / menuSheetTile.h : menuRect.w/2 / menuSheetTile.w;
    if (sf > 1) sf = 1;

    // Draw top layer
    SDL_Rect destRect = {0, 0, int(menuSheetTile.w * sf), int(menuSheetTile.h * sf)};
    for (int r = 0; r < 3; r++) {
        // Fetch tile and return to left side
        menuSheetTile.y = r * menuSheetTile.h;
        menuSheetTile.x = 0;
        destRect.x = 0;

        // Draw TL, Middle, TR
        if (r == 1 && menuRect.h > 2*destRect.h) destRect.h = menuRect.h - 2*destRect.h;
        else destRect.h = int(menuSheetTile.h * sf);
        SDL_RenderCopy(window.renderer, menuSheet, &menuSheetTile, &destRect);

        if (menuRect.w > 2*destRect.w) {
            menuSheetTile.x += menuSheetTile.w;
            destRect.x += destRect.w;
            destRect.w = menuRect.w - 2 * destRect.w;
            SDL_RenderCopy(window.renderer, menuSheet, &menuSheetTile, &destRect);
        }

        destRect.w = int(menuSheetTile.w * sf);
        destRect.x = menuRect.w - destRect.w;
        menuSheetTile.x = 2*menuSheetTile.w;
        SDL_RenderCopy(window.renderer, menuSheet, &menuSheetTile, &destRect);

        destRect.y += destRect.h;
    }

    // reset renderTarget
    SDL_SetRenderTarget(window.renderer, nullptr);

    // Create the button textures
    closeButton->CreateTextures();

    // Close textures
    tm->CloseTexture(MENU_SHEET);
    fm->CloseFont(Font::CONFESSION);

    return true;
}

bool Menu::Display() {
    if (!canDisplay) return false;

    // Display background
    if (showBackground) SDL_RenderCopy(window.renderer, background, nullptr, &menuRect);

    // Display close button
    if (canClose) {
        closeButton->Display();
    }

    return true;
}

bool Menu::UpdatePosition(std::pair<int, int> _position) {
    menuRect.x = _position.first;
    menuRect.y = _position.second;

    return true;
}

SDL_Rect Menu::FetchMenuRect() {
    return menuRect;
}

bool Menu::UpdateSize(std::pair<int, int> _size) {
    menuRect.w = _size.first;
    menuRect.h = _size.second;
}

bool Menu::CloseClicked() {
    if (!canDisplay || !canClose) return false;

    if (closeButton->ClickedStatus()) {
        canDisplay = false;
        return true;
    }

    return false;
}
