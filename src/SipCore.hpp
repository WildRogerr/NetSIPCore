#pragma once
#include <memory>
#include <unordered_map>
#include <pjsua2.hpp>
#include "sip/Account.hpp"
#include "tcp/TcpServer.hpp"
#include "utils/Json.hpp"



class SIPCore {
    
    public:
        SIPCore();
        void init();
        void run();
        void destroy();
        void registerAccount(
            const std::string& server,
            const std::string& username,
            const std::string& password
        );
        void sendState(
            const std::string& username,
            const std::string& state,
            const std::string& remote = ""
        );
        void setupCall(std::shared_ptr<SIPCall> call);
        void handleTcpMessage(const std::string& msg);

    private:
        pj::Endpoint endpoint;
        bool initialized = false;
        std::unique_ptr<SIPAccount> account;
        TCPServer tcpServer;
        std::unordered_map < int,std::shared_ptr <SIPCall> > calls;

};