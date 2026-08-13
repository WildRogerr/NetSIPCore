/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#pragma once
#include <memory>
#include <unordered_map>
#include <pjsua2.hpp>
#include "sip/Account.hpp"
#include "sip/Call.hpp"
#include "sip/InfoModule.h"
#include "tcp/TcpServer.hpp"
#include "utils/Json.hpp"
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <iostream>



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
            const std::string& auth_username,
            const std::string& password,
            const std::string& proxy
        );
        void disconnectAccount(const std::string& username);
        bool setupCall(const std::string& username,std::shared_ptr<SIPCall> call);
        void makeCall(
            const std::string& username,
            const std::string& number,
            const std::string& server
        );
        bool ensureAudioDevice();
        void answerCall(const std::string& username);
        void hangupCall(const std::string& username);
        void processPendingStates();
        void processPendingCommands();
        void sendState(
            const std::string& username,
            const std::string& state,
            const std::string& remote = "",
            const std::string& audio_state = "stop"
        );
        void handleTcpMessage(const std::string& msg);
        void processPendingCallRemove();


    private:
        pj::Endpoint endpoint;
        std::unordered_map<std::string,std::shared_ptr<SIPAccount>> accounts;
        std::unordered_map<std::string,std::shared_ptr<SIPCall>> calls;
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
        struct PendingCommand
        {
            std::string command;
            std::string username;
            std::string auth_username;
            std::string server;
            std::string proxy;
            std::string password;
            std::string remote;
            std::string device;
            std::string audio_path;
        };
        std::queue<PendingCommand> pendingCommands;
        std::mutex commandMutex;
        std::mutex callRemoveMutex;
        std::queue<std::string> pendingCallRemove;
        std::mutex accountsMutex;
        std::mutex callsMutex;
        std::atomic<bool> initialized{false};
        std::atomic<bool> destroyed{false};
        std::atomic<bool> nullDev{false};
        struct DeviceState
        {
            bool microphone = true;
            bool speaker = true;
        };
        std::unordered_map<std::string, DeviceState> deviceStates;
        std::mutex deviceStateMutex;

};
