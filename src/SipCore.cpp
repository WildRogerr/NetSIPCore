#include "SIPCore.hpp"
#include <iostream>



SIPCore::SIPCore() {}


SIPCore::~SIPCore()
{
    destroy();
}


void SIPCore::destroy()
{
    if (!initialized)
    {
        return;
    }

    initialized = false;

    try
    {
        tcpServer.stop();

        accounts.clear();
        calls.clear();

        endpoint.libDestroy();

        std::cout
            << "SIPCore destroyed"
            << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "DESTROY ERROR: "
            << err.info()
            << std::endl;
    }
}


void SIPCore::init()
{
    endpoint.libCreate();

    pj::EpConfig ep_cfg;

    ep_cfg.medConfig.clockRate = 48000;
    ep_cfg.medConfig.sndClockRate = 48000;
    ep_cfg.medConfig.channelCount = 1;
    ep_cfg.medConfig.audioFramePtime = 20;

    endpoint.libInit(ep_cfg);

    pj::TransportConfig transport_cfg;
    transport_cfg.port = 5060;

    endpoint.transportCreate(
        PJSIP_TRANSPORT_UDP,
        transport_cfg
    );

    endpoint.libStart();

    auto& adm =
        endpoint.audDevManager();

    adm.setCaptureDev(1);
    adm.setPlaybackDev(2);

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


void SIPCore::run()
{

    std::cout << "RUN LOOP START" << std::endl;
    while (initialized)
    {
        endpoint.libHandleEvents(10);

        processPendingCommands();

        processPendingStates();

        pj_thread_sleep(10);
    }

}


void SIPCore::registerAccount(
    const std::string& server,
    const std::string& username,
    const std::string& password
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

    accounts[username] = account;

    account->setStateCallback(
        [this](
            const std::string& username,
            const std::string& state,
            const std::string& remote
        )
        {
            std::cout
                << "[STATE CALLBACK] "
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

        account->create(config);

        std::cout
            << "Register request sent"
            << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "PJSIP ERROR: "
            << err.info()
            << std::endl;
    }

}


void SIPCore::disconnectAccount(const std::string& username)
{

    auto it = accounts.find(username);

    if (it == accounts.end())
    {
        return;
    }

    auto callIt = calls.find(username);

    if (callIt != calls.end())
    {
        try
        {
            pj::CallOpParam prm;

            callIt->second->hangup(prm);
        }
        catch (...)
        {
        }

        calls.erase(callIt);

        std::cout
            << "CALL REMOVED: "
            << username
            << std::endl;
    }

    try
    {
        it->second->shutdown();
    }
    catch (...)
    {
    }

    accounts.erase(it);

    sendState(
        username,
        "disconnected"
    );

    std::cout
        << "ACCOUNT REMOVED: "
        << username
        << std::endl;

}


void SIPCore::setupCall(const std::string& username, std::shared_ptr<SIPCall> call)
{

    calls[username] = call;

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
                std::cout
                    << "CALL DISCONNECTED: "
                    << username
                    << std::endl;
            }
        };

    std::cout
        << "CALL STORED: "
        << username
        << std::endl;

}


void SIPCore::makeCall(
    const std::string& username,
    const std::string& number,
    const std::string& server
)
{

    auto it = accounts.find(username);

    if (it == accounts.end())
    {
        return;
    }

    auto call = std::make_shared<SIPCall>(
        *it->second,
        PJSUA_INVALID_ID
    );

    setupCall(
        username,
        call
    );

    pj::CallOpParam prm(true);

    prm.opt.audioCount = 1;
    prm.opt.videoCount = 0;

    std::string uri =
        "sip:" +
        number +
        "@" +
        server;

    try
    {
        call->makeCall(
            uri,
            prm
        );

        std::cout
            << "CALL STARTED: "
            << number
            << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "CALL ERROR: "
            << err.info()
            << std::endl;
    }

}


void SIPCore::answerCall(const std::string& username)
{

    auto it = calls.find(username);

    if (it == calls.end())
    {
        return;
    }

    pj::CallOpParam prm;

    prm.statusCode = PJSIP_SC_OK;

    it->second->answer(prm);

}


void SIPCore::hangupCall(const std::string& username)
{

    auto it =
    calls.find(username);

    if (it == calls.end())
    {
        return;
    }

    pj::CallOpParam prm;

    it->second->hangup(prm);

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
                cmd.password
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
    }
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
        "\"username\":\"" + username + "\","
        "\"state\":\"" + state + "\"";

    if (!cleanRemote.empty())
    {
        json += ",\"remote\":\"" + cleanRemote + "\"";
    }

    json += ",\"audio_state\":\"" + audio_state + "\"";

    json += "}";

    tcpServer.sendMessage(json);

    std::cout
        << "[STATE] "
        << json
        << std::endl;
}


void SIPCore::handleTcpMessage(const std::string& msg)
{
    JsonCommand cmd = Json::parse(msg);

    if (!cmd.valid)
    {
        std::cout
            << "INVALID JSON"
            << std::endl;

        return;
    }

    {
        std::lock_guard<std::mutex> lock(commandMutex);

        pendingCommands.push({
            cmd.command,
            cmd.username,
            cmd.server,
            cmd.password,
            cmd.remote
        });
    }
}