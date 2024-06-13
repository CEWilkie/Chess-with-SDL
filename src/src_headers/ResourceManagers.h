//
// Created by cew05 on 10/06/2024.
//

#ifndef CHESS_WITH_SDL_RESOURCEMANAGERS_H
#define CHESS_WITH_SDL_RESOURCEMANAGERS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <unordered_map>
#include "GlobalResources.h"
#include "GlobalSource.h"

/*
 * Temp defs
 */

enum Texture : int;
enum class Font;

/*
 * Generic ResourceManager
 * used locally within classes to house various resources
 */

template<class Key, class Resource>
class ResourceManager {
    private:

    protected:
        std::unordered_map<Key, Resource>* uoResourceMap;

        Resource errorReturn;

        std::string lastIssue;
        std::vector<std::string> issues {};

    public:
        ResourceManager() {
            uoResourceMap = new std::unordered_map<Key, Resource>;
        }

        bool NewResource(Resource _resourceToAdd, Key _resourceID) {
            // Ensure resource doesnt already exist with ID
            if (uoResourceMap->count(_resourceID) == 1) {
                return false;
            }

            (*uoResourceMap)[_resourceID] = _resourceToAdd;
            return true;
        }

        bool NewOrUpdateExisting(Resource _resource, Key _resourceID) {
            (*uoResourceMap)[_resourceID] = _resource;

            return true;
        }

        Resource FetchResource(Key _resourceID) {
            // Ensure resource exists
            if (uoResourceMap->count(_resourceID) != 1) {
                return errorReturn;
            }

            return (*uoResourceMap)[_resourceID];
        }

        bool FetchResource(Key _resourceID, Resource* _resourcePtr) {
            if (uoResourceMap->count(_resourceID) != 1) {
                return false;
            }

            // Resource exists, set ptr to resource
            _resourcePtr = &(*uoResourceMap)[_resourceID];
            return true;
        }

        bool UpdateExistingResource(Resource _newResource, Key _resourceID) {
            // Ensure resource exists
            if (uoResourceMap->count(_resourceID) != 1) return false;

            (*uoResourceMap)[_resourceID] = _newResource;
            return true;
        }

        bool ResourceExists(Key _resourceID) {
            return (uoResourceMap->count(_resourceID) > 0);
        }
};





/*
 * Global Texture Manager
 * used globally to house all textures and relevant information for constructing them
 */

struct TextureInfo {
    std::string path {};
    unsigned int count = 0;
};

class TextureManager : protected ResourceManager<Texture, TextureInfo> {
    private:
        // Constructing Singleton instance
        static TextureManager* rmInstance;
        TextureManager();
        ~TextureManager();

        // unordered map object to store texture resources in
        std::unordered_map<Texture, SDL_Texture*>* uoTextureMap;

        // Loading and Unloading textures
        bool LoadTexture(Texture _textureID);
        bool UnloadTexture(Texture _textureID);

        // Loading specific texture style

    public:
        // Constructing Singleton instance
        static TextureManager* GetInstance();
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        // Texture management
        bool NewTexture(const std::string &_texturePath, Texture _textureID);
        bool NewTexture(SDL_Texture* _texture, Texture _textureID);
        bool UpdateTexture(const std::string& _texturePath, Texture _textureID);
        bool UpdateTexture(SDL_Texture* _texture, Texture _textureID);
        SDL_Texture* OpenTexture(Texture _textureID);
        bool CloseTexture(Texture _textureID);
        SDL_Texture* FetchTexture(Texture _textureID);
        bool DestroyTexture(Texture _textureID);
        bool TextureExists(Texture _textureID);
};





/*
 * Global Font Manager
 * used globally to house paths to fonts, and open/close a particular font by ID.
 */

struct FontInfo {
    std::string path {};
    int ptSize = 50;
    unsigned int count = 0;
};

class FontManager : protected ResourceManager<Font, FontInfo> {
    private:
        // Constructing Singleton instance
        static FontManager* fmInstance;
        FontManager();
        ~FontManager();

        // unordered map object to store in-use TTF_Font's in
        std::unordered_map<Font, TTF_Font*>* uoFontMap;

        // Loading and Unloading fonts
        bool LoadFont(Font _fontID);
        bool UnloadFont(Font _fontID);

    public:
        // Constructing Singleton instance
        static FontManager* GetInstance();
        FontManager(const FontManager&) = delete;
        FontManager& operator=(const FontManager&) = delete;

        // Font management
        bool CreateNewFont(const std::string& _fontPath, int _ptSize, Font _fontID);
        bool UpdateExistingFont(const std::string& _newFontPath, int _ptSize, Font _fontID);
        TTF_Font* OpenExistingFont(Font _fontID);
        TTF_Font* OpenNewFont(const std::string& _fontPath, int _ptSize, Font _fontID);
        bool CloseFont(Font _fontID);
        bool DestroyFont();
};

#endif //CHESS_WITH_SDL_RESOURCEMANAGERS_H
