#pragma once
#include <memory>
#include <unordered_map>
#include <pjsua2.hpp>
#include "sip/Account.hpp"
#include "sip/Call.hpp"
#include "tcp/TcpServer.hpp"
#include "utils/Json.hpp"
#include <queue>
#include <mutex>


class SIPCore {
    
    public:
        SIPCore();
        ~SIPCore();
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
        void processPendingStates();
        void sendState(
            const std::string& username,
            const std::string& state,
            const std::string& remote = "",
            const std::string& audio_state = "stop"
        );
        void handleTcpMessage(const std::string& msg);


    private:
        pj::Endpoint endpoint;
        bool initialized = false;
        std::unordered_map < std::string,std::shared_ptr <SIPAccount>> accounts;
        std::unordered_map < std::string,std::shared_ptr <SIPCall>> calls;
        TCPServer tcpServer;
        struct PendingState
        {
            std::string username;
            std::string state;
            std::string remote;
            std::string audio_state;
        };
        std::queue<PendingState> pendingStates;
        std::mutex pendingMutex;

};