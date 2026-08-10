/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include "SIPCore.hpp"



SIPCore::SIPCore() {}


SIPCore::~SIPCore()
{
    destroy();
}


void SIPCore::init()
{
    endpoint.libCreate();

    pj::EpConfig ep_cfg;
    ep_cfg.medConfig.clockRate = 44100;
    ep_cfg.medConfig.sndClockRate = 44100;
    ep_cfg.medConfig.channelCount = 1;
    ep_cfg.medConfig.audioFramePtime = 20;
    ep_cfg.medConfig.noVad = true;
    // ep_cfg.logConfig.level = 6;
    // ep_cfg.logConfig.consoleLevel = 6;

    endpoint.libInit(ep_cfg);
    
    pj::TransportConfig transport_cfg;
    
    try
    {
        transport_cfg.port = 5060;
        endpoint.transportCreate(
            PJSIP_TRANSPORT_UDP,
            transport_cfg
        );
    }
    catch (...)
    {
        transport_cfg.port = 0;
        endpoint.transportCreate(
            PJSIP_TRANSPORT_UDP,
            transport_cfg
        );
    }

    endpoint.libStart();

    endpoint.codecSetPriority("PCMA/8000/1", 255);
    endpoint.codecSetPriority("PCMU/8000/1", 254);
    endpoint.codecSetPriority("G722/16000/1", 253);
    endpoint.codecSetPriority("speex/8000/1", 252);
    endpoint.codecSetPriority("speex/16000/1", 251);
    endpoint.codecSetPriority("speex/32000/1", 250);
    endpoint.codecSetPriority("GSM/8000/1", 249);
    endpoint.codecSetPriority("iLBC/8000/1", 248);
    endpoint.codecSetPriority("L16/44100/1", 247);
    endpoint.codecSetPriority("L16/44100/2", 246);
    
    InfoModule::init();

    initialized = true;

    std::cout
        << "SIPCore initialized"
        << std::endl;

    tcpServer.init();

    tcpServer.setOnMessage(
        [this](const std::string& msg)
        {
            handleTcpMessage(msg);
        }
    );

    tcpServer.run(4890);

}


void SIPCore::destroy()
{
    initialized = false;

    if (destroyed)
        return;

    destroyed = true;

    try
    {
        endpoint.hangupAllCalls();

        for (int i = 0; i < 20; i++)
        {
            endpoint.libHandleEvents(50);
        }

        tcpServer.stop();

        endpoint.libHandleEvents(100);

        {
            std::lock_guard<std::mutex> lock(callsMutex);
            calls.clear();
        }

        {
            std::lock_guard<std::mutex> lock(accountsMutex);
            accounts.clear();
        }

        endpoint.libDestroy();

        std::cout
            << "SIPCore destroyed"
            << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "Destroy error: "
            << err.info()
            << std::endl;
    }
}


void SIPCore::run()
{

    std::cout << "Run loop start" << std::endl;
    while (initialized)
    {
        endpoint.libHandleEvents(10);

        processPendingCommands();

        processPendingStates();

        processPendingCallRemove();

        pj_thread_sleep(10);
    }

}


void SIPCore::registerAccount(
    const std::string& server,
    const std::string& username,
    const std::string& password,
    const std::string& proxy
)
{

    if (!initialized)
    {
    std::cout
    << "ERROR: SIPCore not initialized"
    << std::endl;

        return;
    }

    auto account = std::make_shared<SIPAccount>();

    account->setStateCallback(
        [this](
            const std::string& username,
            const std::string& state,
            const std::string& remote
        )
        {
            std::cout
                << "STATE CALLBACK "
                << username
                << " -> "
                << state
                << std::endl;

            sendState(
                username,
                state,
                remote
            );
        }
    );

    account->setCallCallback(
        [this, username](
            std::shared_ptr<SIPCall> call
        )
        {
            setupCall(
                username,
                call
            );
        }
    );

    try
    {
        pj::AccountConfig config;

        config.idUri =
            "sip:" +
            username +
            "@" +
            server;

        config.regConfig.registrarUri =
            "sip:" + server;

        if (!proxy.empty())
        {
            config.sipConfig.proxies.push_back(
                "sip:" + proxy
            );
        }

        pj::AuthCredInfo cred(
            "digest",
            "*",
            username,
            0,
            password
        );

        config.sipConfig.authCreds.push_back(
            cred
        );

        {
            std::lock_guard<std::mutex> lock(accountsMutex);

            if (accounts.find(username) != accounts.end())
            {
                std::cout
                    << "Account already registered: "
                    << username
                    << std::endl;

                return;
            }
        }

        account->create(config);

        {
            std::lock_guard<std::mutex> lock(accountsMutex);
            accounts[username] = account;
        }

        std::cout
            << "Register request sent"
            << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "PJSIP error: "
            << err.info()
            << std::endl;
    }

}


void SIPCore::disconnectAccount(const std::string& username)
{

    std::shared_ptr<SIPAccount> account;
    std::shared_ptr<SIPCall> call;


    {
        std::lock_guard<std::mutex> lock(accountsMutex);

        auto it = accounts.find(username);

        if(it == accounts.end())
            return;

        account = it->second;
    }


    {
        std::lock_guard<std::mutex> lock(callsMutex);

        auto it = calls.find(username);

        if(it != calls.end())
            call = it->second;
    }


    if(call)
    {
        try
        {
            pj::CallOpParam prm;
            call->hangup(prm);
        }
        catch(...)
        {
        }
    }

    if(account)
    {
        try
        {
            account->shutdown();
        }
        catch(...)
        {
        }
    }

    {
        std::lock_guard<std::mutex> lock(accountsMutex);
        accounts.erase(username);
    }

    account.reset();
    call.reset();

    sendState(
        username,
        "disconnected"
    );

    std::cout
        << "Account removed: "
        << username
        << std::endl;

}


void SIPCore::setupCall(const std::string& username, std::shared_ptr<SIPCall> call)
{

    {
        std::lock_guard<std::mutex> lock(callsMutex);
        calls[username] = call;
    }

    {
        std::lock_guard<std::mutex> lock(deviceStateMutex);

        auto it = deviceStates.find(username);

        if (it != deviceStates.end())
        {
            call->setMicrophoneState(it->second.microphone);
            call->setSpeakerState(it->second.speaker);
        }
    }

    call->stateCallback =
        [this, username](
            const std::string& local,
            const std::string& state,
            const std::string& remote
        )
        {
            {
                std::lock_guard<std::mutex> lock(pendingMutex);

                pendingStates.push({
                    username,
                    state,
                    remote,
                    "stop"
                });
                
            }

            if (state == "disconnected")
            {   
                std::lock_guard<std::mutex> lock(callRemoveMutex);
                pendingCallRemove.push(username);
                std::cout
                    << "Call disconnected: "
                    << username
                    << std::endl;
            }
        };

    std::cout
        << "Call stored: "
        << username
        << std::endl;

}


void SIPCore::makeCall(
    const std::string& username,
    const std::string& number,
    const std::string& server
)
{
    std::shared_ptr<SIPAccount> account;

    {
        std::lock_guard<std::mutex> lock(accountsMutex);

        auto it = accounts.find(username);

        if(it == accounts.end())
            return;

        account = it->second;
    }

    auto call = std::make_shared<SIPCall>(
        *account,
        PJSUA_INVALID_ID
    );

    setupCall(username,call);

    std::string uri =
        "sip:" +
        number +
        "@" +
        server;

    ensureAudioDevice();

    pj::CallOpParam prm(true);

    prm.opt.audioCount = 1;
    prm.opt.videoCount = 0;
    prm.opt.textCount = 0;

    try
    {
        call->makeCall(uri,prm);

        std::cout
            << "Call started: "
            << number
            << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "Call error: "
            << err.info()
            << std::endl;
    }

}


bool SIPCore::ensureAudioDevice()
{
    if (nullDev)
        return false;

    try
    {
        auto& adm = endpoint.audDevManager();

        int captureDev = adm.getCaptureDev();
        int playbackDev = adm.getPlaybackDev();

        adm.getDevInfo(captureDev);
        adm.getDevInfo(playbackDev);

        return true;
    }
    catch (const pj::Error&)
    {
        std::cout
            << "No usable audio device, switching to Null Audio Device"
            << std::endl;

        try
        {
            endpoint.audDevManager().setNullDev();
            nullDev = true;
            return false;
        }
        catch (const pj::Error& err)
        {
            std::cout
                << "Failed to set Null Audio Device: "
                << err.info()
                << std::endl;

            return false;
        }
    }
}


void SIPCore::answerCall(const std::string& username)
{
    std::shared_ptr<SIPCall> call;
    {
        std::lock_guard<std::mutex> lock(callsMutex);

        auto it = calls.find(username);

        if(it == calls.end())
            return;

        call = it->second;
    }

    ensureAudioDevice();

    pj::CallOpParam prm;

    prm.statusCode = PJSIP_SC_OK;

    try
    {
        call->answer(prm);
    }
    catch(...)
    {
    }

}


void SIPCore::hangupCall(const std::string& username)
{

    std::shared_ptr<SIPCall> call;
    {
        std::lock_guard<std::mutex> lock(callsMutex);

        auto it = calls.find(username);

        if(it == calls.end())
            return;

        call = it->second;
    }

    pj::CallOpParam prm;

    try
    {
        call->hangup(prm);
    }
    catch(...)
    {
    }

}


void SIPCore::processPendingCallRemove()
{
    std::queue<std::string> q;

    {
        std::lock_guard<std::mutex> lock(callRemoveMutex);
        std::swap(q, pendingCallRemove);
    }

    while (!q.empty())
    {
        auto username = q.front();
        q.pop();

        {
            std::lock_guard<std::mutex> lock(callsMutex);
            calls.erase(username);
        }

        std::cout
            << "Call removed: "
            << username
            << std::endl;
    }
}


void SIPCore::processPendingStates()
{
    std::queue<PendingState> localQueue;

    {
        std::lock_guard<std::mutex> lock(pendingMutex);
        std::swap(localQueue, pendingStates);
    }

    while (!localQueue.empty())
    {
        auto item = localQueue.front();
        localQueue.pop();

        sendState(
            item.username,
            item.state,
            item.remote,
            item.audio_state
        );
    }
}


void SIPCore::processPendingCommands()
{
    std::queue<PendingCommand> localQueue;

    {
        std::lock_guard<std::mutex> lock(commandMutex);
        std::swap(localQueue, pendingCommands);
    }

    while (!localQueue.empty())
    {
        auto cmd = localQueue.front();
        localQueue.pop();

        if (cmd.command == "registration")
        {
            registerAccount(
                cmd.server,
                cmd.username,
                cmd.password,
                cmd.proxy
            );
        }

        else if (cmd.command == "disconnect")
        {
            disconnectAccount(cmd.username);
        }

        else if (cmd.command == "call")
        {
            makeCall(
                cmd.username,
                cmd.remote,
                cmd.server
            );
        }

        else if (cmd.command == "answer")
        {
            answerCall(cmd.username);
        }

        else if (cmd.command == "hangup")
        {
            hangupCall(cmd.username);
        }

        else if (cmd.command == "mute")
        {   

            {
                std::lock_guard<std::mutex> lock(deviceStateMutex);

                if (cmd.device == "microphone")
                    deviceStates[cmd.username].microphone = false;

                if (cmd.device == "speaker")
                    deviceStates[cmd.username].speaker = false;
            }

            std::shared_ptr<SIPCall> call;

            {
                std::lock_guard<std::mutex> lock(callsMutex);

                auto it = calls.find(cmd.username);

                if (it != calls.end())
                    call = it->second;
            }

            if (call)
            {
                if (cmd.device == "microphone")
                    call->setMicrophoneState(false);

                if (cmd.device == "speaker")
                    call->setSpeakerState(false);
            }

        }

        else if (cmd.command == "unmute")
        {

            {
                std::lock_guard<std::mutex> lock(deviceStateMutex);

                if (cmd.device == "microphone")
                    deviceStates[cmd.username].microphone = true;

                if (cmd.device == "speaker")
                    deviceStates[cmd.username].speaker = true;
            }

            std::shared_ptr<SIPCall> call;

            {
                std::lock_guard<std::mutex> lock(callsMutex);

                auto it = calls.find(cmd.username);

                if (it != calls.end())
                    call = it->second;
            }

            if (call)
            {
                if (cmd.device == "microphone")
                    call->setMicrophoneState(true);

                if (cmd.device == "speaker")
                    call->setSpeakerState(true);
            }

        }

        else if (cmd.command == "send_audio")
        {
            std::shared_ptr<SIPCall> call;

            {
                std::lock_guard<std::mutex> lock(callsMutex);

                auto it = calls.find(cmd.username);

                if (it != calls.end())
                    call = it->second;
            }

            if (!call)
                continue;

            call->playAudio(
                cmd.audio_path,
                [this, username = cmd.username, call]()
                {
                    std::lock_guard<std::mutex> lock(pendingMutex);

                    pendingStates.push({
                        username,
                        "streaming",
                        call->getRemoteUri(),
                        "stop"
                    });
                }
            );

        }

        else if (cmd.command == "destroy")
        {
            initialized = false;
        }

    }
}


static std::string escapeJson(const std::string& value)
{
    std::string result;

    for (unsigned char c : value)
    {
        switch (c)
        {
            case '"':
                result += "\\\"";
                break;

            case '\\':
                result += "\\\\";
                break;

            case '\b':
                result += "\\b";
                break;

            case '\f':
                result += "\\f";
                break;

            case '\n':
                result += "\\n";
                break;

            case '\r':
                result += "\\r";
                break;

            case '\t':
                result += "\\t";
                break;

            default:
                if (c < 0x20)
                {
                    char buffer[7];

                    std::snprintf(
                        buffer,
                        sizeof(buffer),
                        "\\u%04x",
                        c
                    );

                    result += buffer;
                }
                else
                {
                    result += c;
                }

                break;
        }
    }

    return result;
}


void SIPCore::sendState(
    const std::string& username,
    const std::string& state,
    const std::string& remote,
    const std::string& audio_state
)
{
    std::string cleanRemote;

    if (!remote.empty())
    {
        size_t sipPos = remote.find("sip:");

        if (sipPos != std::string::npos)
        {
            sipPos += 4;

            size_t endPos = remote.find('@', sipPos);

            if (endPos != std::string::npos)
            {
                cleanRemote =
                    remote.substr(
                        sipPos,
                        endPos - sipPos
                    );
            }
        }
    }

    std::string json =
        "{"
        "\"username\":\"" + escapeJson(username) + "\","
        "\"state\":\"" + escapeJson(state) + "\"";

    if (!cleanRemote.empty())
    {
        json +=
            ",\"remote\":\"" +
            escapeJson(cleanRemote) +
            "\"";
    }

    json +=
        ",\"audio_state\":\"" +
        escapeJson(audio_state) +
        "\"";

    json += "}";

    tcpServer.sendMessage(json);

    // std::cout << "STATE " << json << std::endl;
}


void SIPCore::handleTcpMessage(const std::string& msg)
{
    JsonCommand cmd = Json::parse(msg);

    if (!cmd.valid)
    {
        std::cout
            << "Invalid json"
            << std::endl;

        return;
    }

    {
        std::lock_guard <std::mutex> lock(commandMutex);

        pendingCommands.push({
            cmd.command,
            cmd.username,
            cmd.server,
            cmd.proxy,
            cmd.password,
            cmd.remote,
            cmd.device,
            cmd.audio_path
        });
    }
}
