//
// Created by cew05 on 26/06/2024.
//

#ifndef CHESS_WITH_SDL_STOCKFISHMANAGER_H
#define CHESS_WITH_SDL_STOCKFISHMANAGER_H

#include <iostream>
#include <windows.h>

class StockfishManager {
    private:
        // Pipes to process
        SECURITY_ATTRIBUTES secAttr;
        HANDLE wbOutputPipe {};
        HANDLE rbOutputPipe {};
        HANDLE wbInputPipe {};
        HANDLE rbInputPipe {};

        std::string inputPipeContents {};

        static const DWORD bufferSize = 256;
        DWORD bytesRead = 0;
        DWORD bytesWritten = 0;

        // Stockfish Process
        PROCESS_INFORMATION pi {};
        STARTUPINFO si {};

        //

    public:
        StockfishManager();
        ~StockfishManager();

        bool DoFunction(const std::string& _cmd);
        std::string FetchResult();
};

#endif //CHESS_WITH_SDL_STOCKFISHMANAGER_H
