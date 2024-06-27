//
// Created by cew05 on 26/06/2024.
//

#include "StockfishManager.h"

StockfishManager::StockfishManager() {
    // Set Security Attributes
    secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    secAttr.bInheritHandle = true;
    secAttr.lpSecurityDescriptor = nullptr;

    // Create Pipes
    if (!CreatePipe(&rbOutputPipe, &wbOutputPipe, &secAttr, 0)) {
        printf("Failed to create pipe: %lu\n", GetLastError());
        return;
    }
    if (!CreatePipe(&rbInputPipe, &wbInputPipe, &secAttr, 0)) {
        printf("Failed to create pipe: %lu\n", GetLastError());
        return;
    }

    // Set Startup Info
    si.cb = sizeof(si);
    si.hStdOutput = wbOutputPipe;
    si.hStdInput = rbInputPipe;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcessA("../stockfish/stockfish-windows-x86-64-avx2.exe",
                        nullptr,
                        nullptr,
                        nullptr,
                        true,
                        CREATE_NO_WINDOW,
                        nullptr,
                        nullptr,
                        &si,
                        &pi)) {
        printf("Failed to create stockfish process. EC: %lu\n", GetLastError());
        return;
    }

    // Fetch initialisation string to confirm success
    printf("INIT : %s\n", FetchResult().c_str());

    DoFunction("uci\n");
    printf("%s\n", FetchResult().c_str());
    DoFunction("isready\n");
    printf("%s\n", FetchResult().c_str());
}

StockfishManager::~StockfishManager() {
    // Run quit command
    DoFunction("quit\n");

    // Ensure that the program has closed
    auto result = WaitForSingleObject(pi.hProcess, 600);
    if (result != WAIT_OBJECT_0) {
        // took too long to close
        printf("Termination took too long, force terminate.\n");
        TerminateProcess(pi.hProcess, EXIT_FAILURE);
    }

    // Close handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("NOTICE: STOCKFISH CLOSED");
}

bool StockfishManager::DoFunction(const std::string& _cmd) {
    // Turn string command into char list
    char cmd[_cmd.length()];
    for (int c = 0; c < _cmd.length(); c++) {
        cmd[c] = _cmd[c];
    }

    //printf("REMOVETHISMESSAGE recieved command [%s]\n", _cmd.c_str());

    // Now send the command
    if (WriteFile(wbInputPipe, cmd, sizeof(cmd), &bytesWritten, nullptr) != TRUE) {
        printf("Failed to write to pipe: %lu\n", GetLastError());
    }

    //printf("REMOVETHISMESSAGE sent command [%s]\n", cmd);

    return true;
}

std::string StockfishManager::FetchResult() {
    // output buffer
    std::string response {};
    bool end = false;

    while (!end) {
        std::string contentsString {};
        char contents[bufferSize] {0};
        if (ReadFile(rbOutputPipe, &contents, bufferSize, &bytesRead, nullptr) != TRUE) {
            printf("Failed to read pipe: %lu\n", GetLastError());
            return {};
        }

        for (char c : contents) {
            if (c!=0) contentsString += c;
        }

        // Check end conditions
        if (contentsString.length() < bufferSize) end = true;
        if (contentsString.length() > bufferSize) {
            contentsString.erase(bufferSize, std::string::npos);
        }
        response += contentsString;
    }

    return response;
}

