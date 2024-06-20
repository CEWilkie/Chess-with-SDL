////
//// Created by cew05 on 10/06/2024.
////
//
//#include "ResourceManagers.h"
//
//TextureManager* TextureManager::rmInstance = TextureManager::GetInstance();
//
//TextureManager* TextureManager::GetInstance() {
//    if (!rmInstance) rmInstance = new TextureManager;
//    return rmInstance;
//}
//
//TextureManager::TextureManager() : ResourceManager<TextureID, TextureInfo>(){
//    uoTextureMap = new std::unordered_map<TextureID, SDL_Texture*>;
//}
//
//TextureManager::~TextureManager() = default;
//
///*
// * TextureID Manager definitions
// */
//
//bool TextureManager::LoadTexture(TextureID _textureID) {
//    // Ensure textureID exists
//    if (!TextureExists(_textureID)) return false;
//
//    // If the path is empty, or the texture is not null, consider the texture to already be loaded
//    if ((*uoResourceMap)[_textureID].path.empty()) return true;
//    if ((*uoTextureMap)[_textureID] != nullptr) return true;
//
//    // Load texture
//    SDL_Texture* texture = IMG_LoadTexture(window.renderer, (*uoResourceMap)[_textureID].path.c_str());
//    if (texture == nullptr) {
//        std::string issue = "Failed to load texture from path " + (*uoResourceMap)[_textureID].path;
//        LogError(issue,SDL_GetError(), false);
//        return false;
//    }
//
//    (*uoTextureMap)[_textureID] = texture;
//    return true;
//}
//
//bool TextureManager::UnloadTexture(TextureID _textureID) {
//    // Ensure textureID exists
//    if (!TextureExists(_textureID)) return false;
//
//    // If the path is empty, texture cannot be recreated. Do not unload
//    if ((*uoResourceMap)[_textureID].path.empty()) return false;
//
//    // Unload texture if not already unloaded and set to nullptr;
//    if ((*uoTextureMap)[_textureID] != nullptr) {
//        SDL_DestroyTexture((*uoTextureMap)[_textureID]);
//        (*uoTextureMap)[_textureID] = nullptr;
//    }
//
//    return true;
//}
//
//bool TextureManager::NewTexture(const std::string &_texturePath, TextureID _textureID) {
//    // Ensure textureInfo does not already exist
//    if (TextureExists(_textureID)) return false;
//
//    // set texture to nullptr, and store info
//    (*uoResourceMap)[_textureID] = {_texturePath};
//    (*uoTextureMap)[_textureID] = nullptr;
//    return true;
//}
//
//bool TextureManager::NewTexture(const std::string &_texturePath, std::pair<int, int> _grid, TextureID _textureID) {
//    // Ensure textureInfo does not already exist
//    if (TextureExists(_textureID)) return false;
//
//    // set texture to nullptr, and store info
//    (*uoResourceMap)[_textureID] = {_texturePath, _grid};
//    (*uoTextureMap)[_textureID] = nullptr;
//    return true;
//}
//
//bool TextureManager::NewTexture(SDL_Texture *_texture, TextureID _textureID) {
//    // Ensure texture does not already exist
//    if (TextureExists(_textureID)) return false;
//
//    // no info to store, set texture pointer
//    (*uoResourceMap)[_textureID] = {};
//    (*uoTextureMap)[_textureID] = _texture;
//    return true;
//}
//
//bool TextureManager::UpdateTexture(SDL_Texture *_texture, TextureID _textureID) {
//    // Ensure texture does already exist
//    if (!TextureExists(_textureID)) return false;
//
//    (*uoTextureMap)[_textureID] = _texture;
//    return true;
//}
//
//bool TextureManager::UpdateTexture(const std::string &_texturePath, TextureID _textureID) {
//    // Ensure texture does already exist
//    if (!TextureExists(_textureID)) return false;
//
//    (*uoResourceMap)[_textureID].path = _texturePath;
//    return true;
//}
//
//bool TextureManager::DestroyTexture(TextureID _textureID) {
//    return uoTextureMap->erase(_textureID) > 0;
//}
//
//SDL_Texture* TextureManager::OpenTexture(TextureID _textureID) {
//    // Check if texture does not exist
//    if (uoTextureMap->count(_textureID) != 1) {
//        return nullptr;
//    }
//
//    LoadTexture(_textureID);
//    (*uoResourceMap)[_textureID].count++;
//    return (*uoTextureMap)[_textureID];
//
//}
//
//bool TextureManager::CloseTexture(TextureID _textureID) {
//    // Ensure texture does already exist
//    if (!TextureExists(_textureID)) return false;
//
//    if (((*uoResourceMap)[_textureID].count--) <= 0) {
//        (*uoResourceMap)[_textureID].count = 0;
//        return UnloadTexture(_textureID);
//    }
//
//    return true;
//}
//
//SDL_Texture* TextureManager::FetchTexture(TextureID _textureID) {
//    // Ensure texture exists
//    if (uoTextureMap->count(_textureID) != 1) return nullptr;
//
//    //if ((*uoTextureMap)[_textureID] == nullptr ) printf("TextureID empty!\n");
//
//    return (*uoTextureMap)[_textureID];
//}
//
//TextureInfo TextureManager::FetchTextureInfo(TextureID _textureID) {
//    if (!TextureExists(_textureID)) return {};
//
//    return (*uoResourceMap)[_textureID];
//}
//
//bool TextureManager::TextureExists(TextureID _textureID) {
//    bool exists = (uoTextureMap->count(_textureID) > 0) && (uoResourceMap->count(_textureID) > 0);
//    return exists;
//}
//
///*
// * Font manager: creating a singleton instance
// */
//
//FontManager* FontManager::fmInstance = FontManager::GetInstance();
//
//FontManager* FontManager::GetInstance() {
//    if (!fmInstance) fmInstance = new FontManager;
//    return fmInstance;
//}
//
//FontManager::FontManager() : ResourceManager<Font, FontInfo>(){
//    uoFontMap = new std::unordered_map<Font, TTF_Font*>;
//}
//
//FontManager::~FontManager() = default;
//
///*
// * Font manager definitions
// */
//
//bool FontManager::LoadFont(Font _fontID) {
//    // Ensure font and font info exists
//    if (uoFontMap->count(_fontID) != 1) return false;
//    if (uoResourceMap->count(_fontID) != 1) return false;
//
//    if ((*uoFontMap)[_fontID] == nullptr) {
//        TTF_Font* font = TTF_OpenFont((*uoResourceMap)[_fontID].path.c_str(), (*uoResourceMap)[_fontID].ptSize);
//        if (font == nullptr) {
//            std::string issue = "Failed to load font: " + (*uoResourceMap)[_fontID].path;
//            LogError(issue, SDL_GetError(), false);
//            return false;
//        }
//
//        (*uoFontMap)[_fontID] = font;
//    }
//
//    return true;
//}
//
//bool FontManager::UnloadFont(Font _fontID) {
//    //Ensure Font and font info exists
//    if (uoFontMap->count(_fontID) != 1) return false;
//    if (uoResourceMap->count(_fontID) != 1) return false;
//
//    if ((*uoFontMap)[_fontID] != nullptr) {
//        TTF_CloseFont((*uoFontMap)[_fontID]);
//        (*uoFontMap)[_fontID] = nullptr;
//    }
//
//    return true;
//}
//
//bool FontManager::CreateNewFont(const std::string &_fontPath, int _ptSize, Font _fontID) {
//    // Ensure font not already stored
//    if (uoFontMap->count(_fontID) != 0) return false;
//    if (uoResourceMap->count(_fontID) != 0) return false;
//
//    (*uoResourceMap)[_fontID] = {_fontPath, _ptSize};
//    (*uoFontMap)[_fontID] = nullptr;
//
//    return true;
//}
//
//bool FontManager::UpdateExistingFont(const std::string &_newFontPath, int _ptSize, Font _fontID) {
//    // Ensure font exists
//    if (uoFontMap->count(_fontID) != 1) return false;
//    if (uoResourceMap->count(_fontID) != 1) return false;
//
//    (*uoResourceMap)[_fontID] = {_newFontPath, _ptSize};
//    UnloadFont(_fontID);
//    return true;
//}
//
//TTF_Font* FontManager::OpenExistingFont(Font _fontID) {
//    // Ensure font exists
//    if (uoFontMap->count(_fontID) != 1) return nullptr;
//    if (uoResourceMap->count(_fontID) != 1) return nullptr;
//
//    LoadFont(_fontID);
//    (*uoResourceMap)[_fontID].count++;
//    return (*uoFontMap)[_fontID];
//}
//
//TTF_Font* FontManager::OpenNewFont(const std::string &_fontPath, int _ptSize, Font _fontID) {
//    // Create a new font, and then open it.
//    CreateNewFont(_fontPath, _ptSize, _fontID);
//    return OpenExistingFont(_fontID);
//}
//
//bool FontManager::CloseFont(Font _fontID) {
//    // Ensure font exists
//    if (uoFontMap->count(_fontID) != 1) return false;
//    if (uoResourceMap->count(_fontID) != 1) return false;
//
//    if (((*uoResourceMap)[_fontID].count--) <= 0) {
//        (*uoResourceMap)[_fontID].count = 0;
//        UnloadFont(_fontID);
//    }
//
//    return true;
//}
//
//bool FontManager::DestroyFont() {
//    return true;
//}