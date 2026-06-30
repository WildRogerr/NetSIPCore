#pragma once
#include <functional>
#include <string>
#include <thread>
#include <winsock2.h>
#include <mutex>



class TCPServer
{
public:

    TCPServer();
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
    bool running = false;
    std::mutex clientMutex;

};