//
// Created by cew05 on 19/04/2024.
//

#ifndef CHESS_WITH_SDL_PIECE_H
#define CHESS_WITH_SDL_PIECE_H

#include <vector>
#include <string>
#include <SDL_image.h>
#include <algorithm>

#include "GlobalSource.h"
#include "Board.h"

struct Piece_Info {
    std::string name = "Pawn";
    std::string color = "White";
    char col = 'W';
};

struct AvailableMove{
    Position<char, int> position;
    bool capture;
};

class Piece {
    protected:
        // Game mechanics / position
        Position<char, int> gamepos {};
        std::vector<AvailableMove> valid_moves {};

        // Piece identification
        Piece_Info info {};

        // SDL display
        std::string imgName {};
        SDL_Texture* pieceTexture {};
        SDL_Texture* moveHighlights[3] {};
        SDL_Rect pieceRect {};
        SDL_Rect boardpos_rect {};
        float b_width = 0.1;

        // pawn movements
        bool hasMoved = false;
        bool canPassant = true;
        bool canPromote = true;
        int dir = 1;

        // universal movements
        bool selected = false;

    public:
        Piece(const std::string& _name, const std::string& _color, Position<char, int> _gamepos);
        int CreateTextures();
        void GetRectOfBoardPosition(const Board& board);
        void DisplayPiece();
        void virtual FetchMoves(const std::vector<Piece*> &opp_pieces, const Board& board);
        void Captured();
        void DisplayMoves(const Board& board);
        bool IsClicked();

};


#endif //CHESS_WITH_SDL_PIECE_H
