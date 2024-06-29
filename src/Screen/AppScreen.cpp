//
// Created by cew05 on 16/06/2024.
//

#include "include/AppScreen.h"

/*
 * Generic App screen definitions
 */

AppScreen::AppScreen() {
    screenRect = window.currentRect;

    // Create base states
    stateManager->NewResource(false, SCREEN_CLOSED);
    stateManager->NewResource(false, NO_INPUT);

    // Create texture for background
    textureManager->NewTexture(nullptr, BACKGROUND);
}

bool AppScreen::CreateTextures() {
    // Create Screen background texture
    // ...

    for (auto &menu : *menuManager->AccessMap()) {
        menu.second->CreateTextures();
    }

    // Display buttons
    for (auto &button : *buttonManager->AccessMap()) {
        if (!button.second->Display()) return false;
    }

    return true;
}

bool AppScreen::LoadScreen() {
    // Non functional
    // ...
    return true;
}

bool AppScreen::Display() {
    // Display background
    SDL_SetRenderDrawColor(window.renderer, 0, 150, 150, 255);
    SDL_RenderFillRect(window.renderer, &window.currentRect);
    SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 0);

    // Display menus
    for (auto &menu : *menuManager->AccessMap()) {
        if (!menu.second->Display()) return false;
    }

    // Display buttons
    for (auto &button : *buttonManager->AccessMap()) {
        if (!button.second->Display()) return false;
    }

    return true;
}


void AppScreen::ResizeScreen() {
    // resize menus
    for (auto& menu : *menuManager->AccessMap()) {
        menu.second->UpdateSize({window.currentRect.w, window.currentRect.h});
    }

    // resize buttons
    // ...
}


void AppScreen::UpdateButtonStates() {
    // Update buttons in menus
    for (auto& menu : *menuManager->AccessMap()) {
        menu.second->UpdateButtonStates();
    }

    // Update other buttons
    for (auto& button : *buttonManager->AccessMap()) {
        button.second->UpdateClickedStatus();
    }
}

void AppScreen::HandleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        // Check for window close
        if (event.type == SDL_QUIT) {
            stateManager->ChangeResource(true, SCREEN_CLOSED);
            break;
        }

        // Check other events
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                mouse.MouseDown(true);
                break;
            case SDL_MOUSEBUTTONUP:
                mouse.MouseDown(false);
                break;
            default:
                break;
        }
    }

    mouse.Update();
    //mouse.PrintStates();
}


void AppScreen::CheckButtons() {
    // Child classes will overwrite this function to check button states and execute functions
}

bool AppScreen::FetchScreenState(int _stateID) {
    bool result;
    stateManager->FetchResource(result, _stateID);
    return result;
}
