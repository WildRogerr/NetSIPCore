/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include "TcpServer.hpp"



static inline void socket_close(SOCKET s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}


static inline void socket_shutdown(SOCKET s)
{
#ifdef _WIN32
    shutdown(s, SD_BOTH);
#else
    shutdown(s, SHUT_RDWR);
#endif
}


#ifdef _WIN32
inline const char* sockopt_cast(const void* p)
{
    return reinterpret_cast<const char*>(p);
}
#else
inline const void* sockopt_cast(const void* p)
{
    return p;
}
#endif



TCPServer::TCPServer() {}


TCPServer::~TCPServer()
{
    stop();
}


void TCPServer::init()
{
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
        initializedWsa = true;
    #else
        signal(SIGPIPE, SIG_IGN);
    #endif

    std::cout << "TCP initialized" << std::endl;
}


void TCPServer::run(uint16_t port)
{
    running = true;

    serverThread = std::thread([this, port]()
    {
        serverSocket = socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP
        );

        if (serverSocket == INVALID_SOCKET)
        {
            std::cout
                << "socket() failed"
                << std::endl;

            running = false;

            return;
        }

        sockaddr_in serverAddr{};

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        int opt = 1;
        setsockopt(
            serverSocket,
            SOL_SOCKET,
            SO_REUSEADDR,
            sockopt_cast(&opt),
            sizeof(opt)
        );

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {   
            socket_shutdown(serverSocket);
            socket_close(serverSocket);
            serverSocket = INVALID_SOCKET;
            std::cout
                << "bind() failed"
                << std::endl;

            running = false;

            return;
        }

        if (listen(serverSocket, 1) == SOCKET_ERROR)
        {   
            socket_shutdown(serverSocket);
            socket_close(serverSocket);
            serverSocket = INVALID_SOCKET;
            
            std::cout
                << "listen() failed"
                << std::endl;

            running = false;

            return;
        }

        std::cout
            << "TCP server running on port "
            << port
            << std::endl;

        while (running)
        {
            SOCKET newClient = accept(
                serverSocket,
                nullptr,
                nullptr
            );

            if (newClient == INVALID_SOCKET)
            {
                if (!running)
                    break;
                    
                continue;
            }

            {
                std::lock_guard<std::mutex> lock(clientMutex);

                if (clientSocket != INVALID_SOCKET)
                {   
                    socket_shutdown(newClient);
                    socket_close(newClient);

                    std::cout
                        << "TCP client rejected: already connected"
                        << std::endl;

                    continue;
                }

                clientSocket = newClient;
            }

            std::cout
                << "TCP client connected"
                << std::endl;

            char buffer[4096];

            std::string recvBuffer;

            while (running)
            {
                SOCKET currentClient;

                {
                    std::lock_guard<std::mutex> lock(clientMutex);
                    currentClient = clientSocket;
                }

                if(currentClient == INVALID_SOCKET)
                {
                    break;
                }

                int bytes = recv(
                    currentClient,
                    buffer,
                    sizeof(buffer),
                    0
                );

                if (bytes <= 0)
                {
                    std::cout
                        << "TCP client disconnected"
                        << std::endl;

                    {
                        std::lock_guard<std::mutex> lock(clientMutex);
                        
                        socket_shutdown(clientSocket);
                        socket_close(clientSocket);
                        clientSocket = INVALID_SOCKET;
                    }

                    break;
                }

                recvBuffer.append(buffer, bytes);

                while (true)
                {
                    size_t pos =
                        recvBuffer.find('\n');

                    if (pos == std::string::npos)
                    {
                        break;
                    }

                    std::string msg =
                        recvBuffer.substr(0, pos);

                    recvBuffer.erase(0, pos + 1);

                    // std::cout << "TCP RECEIVE " << msg << std::endl;

                    if (onMessage)
                    {
                        onMessage(msg);
                    }
                }
            }
        }
    });
}


void TCPServer::stop()
{
    running = false;

    if(serverSocket != INVALID_SOCKET)
    {   
        socket_shutdown(serverSocket);
        socket_close(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    {
        std::lock_guard<std::mutex> lock(clientMutex);

        if(clientSocket != INVALID_SOCKET)
        {   
            socket_shutdown(clientSocket);
            socket_close(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
    }

    if(serverThread.joinable())
    {
        serverThread.join();
    }

#ifdef _WIN32
    if(initializedWsa)
    {
        WSACleanup();
        initializedWsa = false;
    }
#endif
}


void TCPServer::setOnMessage(std::function<void(const std::string&)> cb)
{
    onMessage = cb;
}


void TCPServer::sendMessage(const std::string& msg)
{

    SOCKET currentClient;

    {
        std::lock_guard<std::mutex> lock(clientMutex);
        currentClient = clientSocket;
    }

    if (currentClient == INVALID_SOCKET)
    {
        return;
    }

    std::string data = msg + "\n";

    int result = send(
        currentClient,
        data.c_str(),
        (int)data.size(),
        0
    );

    if (result == SOCKET_ERROR)
    {
        std::cout
            << "TCP send failed"
            << std::endl;

        {
            std::lock_guard<std::mutex> lock(clientMutex);

            socket_shutdown(clientSocket);
            socket_close(clientSocket);
            clientSocket = INVALID_SOCKET;
        }

        return;
    }

    // std::cout << "TCP SEND " << msg << std::endl;

}