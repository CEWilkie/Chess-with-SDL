//
// Created by cew05 on 10/06/2024.
//

#include "src_headers/ResourceManagers.h"

TextureManager* TextureManager::rmInstance = TextureManager::GetInstance();
FontManager* FontManager::fmInstance = FontManager::GetInstance();

/*
 * Resource Manager: definitions
 */














/*
 * TextureManager : creating a singleton instance
 */

TextureManager* TextureManager::GetInstance() {
    if (!rmInstance) rmInstance = new TextureManager;
    return rmInstance;
}

TextureManager::TextureManager() : ResourceManager<Texture, TextureInfo>(){
    uoTextureMap = new std::unordered_map<Texture, SDL_Texture*>;
}

TextureManager::~TextureManager() = default;

/*
 * Texture Manager definitions
 */

bool TextureManager::LoadTexture(Texture _textureID) {
    // Ensure textureID exists
    if (uoTextureMap->count(_textureID) == 0) return false;
    if (uoResourceMap->count(_textureID) == 0) return false;

    if ((*uoTextureMap)[_textureID] == nullptr && !(*uoResourceMap)[_textureID].path.empty()) {
        SDL_Texture* texture = IMG_LoadTexture(window.renderer, (*uoResourceMap)[_textureID].path.c_str());
        if (texture == nullptr) {
            std::string issue = "Failed to load texture: " + (*uoResourceMap)[_textureID].path;
            LogError(issue,SDL_GetError(), false);
            return false;
        }

        (*uoTextureMap)[_textureID] = texture;
    }

    return true;
}

bool TextureManager::UnloadTexture(Texture _textureID) {
    if (uoTextureMap->count(_textureID) != 1) return false;

    if ((*uoTextureMap)[_textureID] != nullptr) {
        SDL_DestroyTexture((*uoTextureMap)[_textureID]);
        (*uoTextureMap)[_textureID] = nullptr;
    }

    return true;
}

bool TextureManager::NewTexture(const std::string &_texturePath, Texture _textureID) {
    // Ensure textureInfo does not already exist
    if (uoResourceMap->count(_textureID) != 1) {
        (*uoResourceMap)[_textureID].path = _texturePath;
    }

    // Does not create the texture, only assigns the value to nullptr;
    (*uoTextureMap)[_textureID] = nullptr;
    return true;
}

bool TextureManager::NewTexture(SDL_Texture *_texture, Texture _textureID) {
    // Ensure texture does not already exist
    if (uoResourceMap->count(_textureID) > 0) return false;

    (*uoResourceMap)[_textureID] = {""};
    (*uoTextureMap)[_textureID] = _texture;
    return true;
}

bool TextureManager::UpdateTexture(SDL_Texture *_texture, Texture _textureID) {
    // Ensure texture exists
    if (uoResourceMap->count(_textureID) == 0) return false;

    (*uoTextureMap)[_textureID] = _texture;
    return true;
}

bool TextureManager::UpdateTexture(const std::string &_texturePath, Texture _textureID) {
    // Ensure texture exists
    if (uoResourceMap->count(_textureID) == 0) return false;

    (*uoResourceMap)[_textureID].path = _texturePath;
    return true;
}

SDL_Texture* TextureManager::OpenTexture(Texture _textureID) {
    // Check if texture does not exist
    if (uoTextureMap->count(_textureID) != 1) {
        return nullptr;
    }

    LoadTexture(_textureID);
    (*uoResourceMap)[_textureID].count++;
    return (*uoTextureMap)[_textureID];

}

bool TextureManager::CloseTexture(Texture _textureID) {
    // check if texture does not exist
    if (uoTextureMap->count(_textureID) != 1) {
        return false;
    }

    if (((*uoResourceMap)[_textureID].count--) <= 0) {
        (*uoResourceMap)[_textureID].count = 0;
        return UnloadTexture(_textureID);
    }

    return true;
}

SDL_Texture* TextureManager::FetchTexture(Texture _textureID) {
    // Ensure texture exists
    if (uoTextureMap->count(_textureID) != 1) return nullptr;

    //if ((*uoTextureMap)[_textureID] == nullptr ) printf("Texture empty!\n");

    return (*uoTextureMap)[_textureID];
}

bool TextureManager::DestroyTexture(Texture _textureID) {
    return true;
}

bool TextureManager::TextureExists(Texture _textureID) {
    return (uoTextureMap->count(_textureID) > 0);
}

/*
 * Font manager: creating a singleton instance
 */


FontManager* FontManager::GetInstance() {
    if (!fmInstance) fmInstance = new FontManager;
    return fmInstance;
}

FontManager::FontManager() : ResourceManager<Font, FontInfo>(){
    uoFontMap = new std::unordered_map<Font, TTF_Font*>;
}

FontManager::~FontManager() = default;

/*
 * Font manager definitions
 */

bool FontManager::LoadFont(Font _fontID) {
    // Ensure font and font info exists
    if (uoFontMap->count(_fontID) != 1) return false;
    if (uoResourceMap->count(_fontID) != 1) return false;

    if ((*uoFontMap)[_fontID] == nullptr) {
        TTF_Font* font = TTF_OpenFont((*uoResourceMap)[_fontID].path.c_str(), (*uoResourceMap)[_fontID].ptSize);
        if (font == nullptr) {
            std::string issue = "Failed to load font: " + (*uoResourceMap)[_fontID].path;
            LogError(issue, SDL_GetError(), false);
            return false;
        }

        (*uoFontMap)[_fontID] = font;
    }

    return true;
}

bool FontManager::UnloadFont(Font _fontID) {
    //Ensure Font and font info exists
    if (uoFontMap->count(_fontID) != 1) return false;
    if (uoResourceMap->count(_fontID) != 1) return false;

    if ((*uoFontMap)[_fontID] != nullptr) {
        TTF_CloseFont((*uoFontMap)[_fontID]);
        (*uoFontMap)[_fontID] = nullptr;
    }

    return true;
}

bool FontManager::CreateNewFont(const std::string &_fontPath, int _ptSize, Font _fontID) {
    // Ensure font not already stored
    if (uoFontMap->count(_fontID) != 0) return false;
    if (uoResourceMap->count(_fontID) != 0) return false;

    (*uoResourceMap)[_fontID] = {_fontPath, _ptSize};
    (*uoFontMap)[_fontID] = nullptr;

    return true;
}

bool FontManager::UpdateExistingFont(const std::string &_newFontPath, int _ptSize, Font _fontID) {
    // Ensure font exists
    if (uoFontMap->count(_fontID) != 1) return false;
    if (uoResourceMap->count(_fontID) != 1) return false;

    (*uoResourceMap)[_fontID] = {_newFontPath, _ptSize};
    UnloadFont(_fontID);
    return true;
}

TTF_Font* FontManager::OpenExistingFont(Font _fontID) {
    // Ensure font exists
    if (uoFontMap->count(_fontID) != 1) return nullptr;
    if (uoResourceMap->count(_fontID) != 1) return nullptr;

    LoadFont(_fontID);
    (*uoResourceMap)[_fontID].count++;
    return (*uoFontMap)[_fontID];
}

TTF_Font* FontManager::OpenNewFont(const std::string &_fontPath, int _ptSize, Font _fontID) {
    // Create a new font, and then open it.
    CreateNewFont(_fontPath, _ptSize, _fontID);
    return OpenExistingFont(_fontID);
}

bool FontManager::CloseFont(Font _fontID) {
    // Ensure font exists
    if (uoFontMap->count(_fontID) != 1) return false;
    if (uoResourceMap->count(_fontID) != 1) return false;

    if (((*uoResourceMap)[_fontID].count--) <= 0) {
        (*uoResourceMap)[_fontID].count = 0;
        UnloadFont(_fontID);
    }

    return true;
}

bool FontManager::DestroyFont() {
    return true;
}