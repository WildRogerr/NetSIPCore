#pragma once
#include <memory>
#include <unordered_map>
#include <pjsua2.hpp>
#include "sip/Account.hpp"
#include "sip/Call.hpp"
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
        void disconnectAccount(const std::string& username);
        void setupCall(const std::string& username,std::shared_ptr<SIPCall> call);
        void SIPCore::makeCall(
            const std::string& username,
            const std::string& number,
            const std::string& server
        );
        void answerCall(const std::string& username);
        void hangupCall(const std::string& username);
        void sendState(
            const std::string& username,
            const std::string& state,
            const std::string& remote = ""
        );
        void handleTcpMessage(const std::string& msg);

    private:
        pj::Endpoint endpoint;
        bool initialized = false;
        std::unordered_map < std::string,std::shared_ptr <SIPAccount>> accounts;
        std::unordered_map < std::string,std::shared_ptr <SIPCall>> calls;
        TCPServer tcpServer;

};