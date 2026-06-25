#include "TCPServer.hpp"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")



TCPServer::TCPServer()
{
}


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
            clientSocket = accept(
                serverSocket,
                nullptr,
                nullptr
            );

            if (clientSocket == INVALID_SOCKET)
            {
                continue;
            }

            std::cout
                << "TCP client connected"
                << std::endl;

            char buffer[4096];

            std::string recvBuffer;

            while (running)
            {
                int bytes = recv(
                    clientSocket,
                    buffer,
                    sizeof(buffer),
                    0
                );

                if (bytes <= 0)
                {
                    std::cout
                        << "TCP client disconnected"
                        << std::endl;

                    closesocket(clientSocket);

                    clientSocket = INVALID_SOCKET;

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

                    std::cout
                        << "[TCP RECEIVE] "
                        << msg
                        << std::endl;

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
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

}


void TCPServer::setOnMessage(std::function<void(const std::string&)> cb)
{
    onMessage = cb;
}


void TCPServer::sendMessage(const std::string& msg)
{

    if (clientSocket == INVALID_SOCKET)
    {
        return;
    }

    std::string data = msg + "\n";

    ::send(
        clientSocket,
        data.c_str(),
        (int)data.size(),
        0
    );

    std::cout
        << "[TCP SEND] "
        << msg
        << std::endl;
        
}