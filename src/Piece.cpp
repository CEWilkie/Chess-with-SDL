//
// Created by cew05 on 19/04/2024.
//

#include "Piece.h"

Piece::Piece(const std::string& _imgPath, Position _gamepos) {
    // Create Texture of Piece
    imgPath = _imgPath;
    texture = IMG_LoadTexture(window.renderer, imgPath.c_str());

    // Set initial game board position
    gamepos = _gamepos;
}