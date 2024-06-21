//
// Created by cew05 on 21/06/2024.
//

#include "include/GameScreen.h"

GameScreen::GameScreen() : AppScreen() {
    // Construct board
    board = new Board();

    // Ensure GameData directory exists else end program
    if (!board->GameDataDirectoryExists()) {
        printf("Failed to ensure game directory exists");
        return;
    }
    board->ClearExcessGameFiles();
    // Create game data files in dir
    if (!board->CreateGameFiles()) {
        printf("Error creating game files.\n");
        return;
    }

    // Construct pieces
    SetUpPieces();
    board->WriteStartPositionsToFile(*allPieces);

    teamptr = whitePieces;
    oppptr = blackPieces;
}


void GameScreen::SetUpPieces() {
    // Read standard board from file
    std::fstream boardStandardFile("../RequiredFiles/BasicSetup.csv");
    std::string pieceString;

    std::vector<std::string> pieceElements;
    char div = ',';
    uint64_t splitpos;

    while (std::getline(boardStandardFile, pieceString)) {
        pieceElements.clear();

        while ((splitpos = pieceString.find(div)) != std::string::npos) {
            pieceElements.push_back(pieceString.substr(0, splitpos));
            pieceString.erase(0, splitpos + 1);
        }
        pieceElements.push_back(pieceString);

        // construct Piece from elements taken from file
        Piece* newPiece = nullptr;
        if (pieceElements[1] == "Pawn") {
            newPiece = new Piece("Pawn", 'W', {});
            newPiece = new Piece("Pawn", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Knight") {
            newPiece = new Knight("Knight", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Bishop") {
            newPiece = new Bishop("Bishop", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Rook") {
            newPiece = new Rook("Rook", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "King") {
            newPiece = new King("King", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Queen") {
            newPiece = new Queen("Queen", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }

        if (newPiece != nullptr) {
            newPiece->CreateTextures();
            newPiece->GetRectOfBoardPosition(*board);
            (pieceElements[0][0] == 'W') ? whitePieces->push_back(newPiece) : blackPieces->push_back(newPiece);
            allPieces->push_back(newPiece);
        }
    }
    boardStandardFile.close();

    printf("CONSTRUCTED %zu WHITE PIECES, %zu BLACK PIECES, %zu TOTAL PIECES\n",
           whitePieces->size(), blackPieces->size(), allPieces->size());
}


bool GameScreen::CreateTextures() {
    if (!AppScreen::CreateTextures()) return false;

    // Create board textures
    board->CreateBoardTexture();

    // Create piece textures
    for (auto& piece : *allPieces) {
        piece->CreateTextures();
    }

    return true;
}

bool GameScreen::Display() {
    // Display screen background
    SDL_SetRenderDrawColor(window.renderer, 0, 150, 150, 255);
    SDL_RenderFillRect(window.renderer, &window.currentRect);
    SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 0);

    // Display board
    board->DisplayGameBoard();

    // Display Pieces and fetch their moves
    for (Piece* piece : *allPieces) {
        piece->ClearMoves();
        piece->ClearNextMoves();
        piece->FetchMoves(*teamptr, *oppptr, *board);
        piece->PreventMoveIntoCheck(*teamptr, *oppptr, *board);

        piece->DisplayPiece();
        piece->DisplayMoves(*board);
    }

    return true;
}

void GameScreen::ResizeScreen() {
    AppScreen::ResizeScreen();

    // Resize board
    board->FillToBounds(window.currentRect.w, window.currentRect.h);

    // Resize pieces
}

void GameScreen::HandleEvents() {
    AppScreen::HandleEvents();

    // Check for stalemate / checkmate
    bool canMove = std::any_of(teamptr->begin(), teamptr->end(), [](Piece *piece) {
        return !piece->GetAvailableMovesPtr()->empty();
    });

    if (!canMove) {
        // if there are no moves available, check if King is being checked by opp
        if (std::any_of(oppptr->begin(), oppptr->end(), [](Piece *piece) {
            return piece->IsCheckingKing();
        })) {
            // conditions met: checkmate
            printf("CHECKMATE! 1:0");
        } else {
            // conditions met: stalemate
            printf("STALEMATE! 0.5:0.5");
        }

        // Show results, store final game results, wait for input to return to menu
        // ...
    }


    // Handle input for moves


}

void GameScreen::CheckButtons() {
    AppScreen::CheckButtons();
}
