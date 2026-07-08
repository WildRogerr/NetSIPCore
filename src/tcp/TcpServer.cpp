#include "TCPServer.hpp"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")



TCPServer::TCPServer() {}


void TCPServer::init()
{
    WSADATA wsaData;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    std::cout
        << "TCP initialized"
        << std::endl;
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

            return;
        }

        sockaddr_in serverAddr{};

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cout
                << "bind() failed"
                << std::endl;

            return;
        }

        if (listen(serverSocket, 1) == SOCKET_ERROR)
        {
            std::cout
                << "listen() failed"
                << std::endl;

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
                continue;
            }

            {
                std::lock_guard<std::mutex> lock(clientMutex);

                if (clientSocket != INVALID_SOCKET)
                {
                    closesocket(newClient);

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

                        closesocket(clientSocket);
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

                    // std::cout << "[TCP RECEIVE] " << msg << std::endl;

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

    {
        std::lock_guard<std::mutex> lock(clientMutex);

        if (clientSocket != INVALID_SOCKET)
        {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
    }

    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    if (serverThread.joinable())
    {
        serverThread.join();
    }

    WSACleanup();
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

            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }

        return;
    }

    // std::cout << "[TCP SEND] " << msg << std::endl;

}