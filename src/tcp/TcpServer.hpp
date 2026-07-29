/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#pragma once
#include <functional>
#include <string>
#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>



#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <signal.h>
    using SOCKET = int;
    constexpr int INVALID_SOCKET = -1;
    constexpr int SOCKET_ERROR = -1;
#endif



class TCPServer
{
public:

    TCPServer();
    ~TCPServer();
    void init();
    void run(uint16_t port);
    void stop();
    void setOnMessage(std::function<void(const std::string&)> cb);
    void sendMessage(const std::string& msg);

private:

    SOCKET serverSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    std::function<void(const std::string&)> onMessage;
    std::thread serverThread;
    std::atomic<bool> running{false};
    std::mutex clientMutex;
    bool initializedWsa = false;

};