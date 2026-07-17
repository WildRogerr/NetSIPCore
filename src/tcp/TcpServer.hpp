#pragma once
#include <functional>
#include <string>
#include <thread>
#include <mutex>


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
    signal(SIGPIPE, SIG_IGN);
#endif



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