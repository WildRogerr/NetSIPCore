#pragma once
#include <memory>
#include <pjsua2.hpp>
#include "sip/Account.hpp"
#include "tcp/TcpServer.hpp"



class SIPCore {
    
    public:
        SIPCore();
        void init();
        void run();
        void destroy();
        void registerAccount(
            const std::string& server,
            const std::string& login,
            const std::string& password
        );
        void handleTcpMessage(const std::string& msg);

    private:
        pj::Endpoint endpoint;
        bool initialized = false;
        std::unique_ptr<SIPAccount> account;
        TCPServer tcpServer;
};