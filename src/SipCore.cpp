#include "SIPCore.hpp"
#include <iostream>



SIPCore::SIPCore() {}


void SIPCore::init()
{

    endpoint.libCreate();

    pj::EpConfig ep_cfg;
    endpoint.libInit(ep_cfg);

    pj::TransportConfig transport_cfg;
    transport_cfg.port = 5060;

    endpoint.transportCreate(PJSIP_TRANSPORT_UDP, transport_cfg);

    endpoint.libStart();

    initialized = true;

    std::cout << "SIPCore initialized" << std::endl;

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
        pj_thread_sleep(10);
    }

}


void SIPCore::destroy()
{

    tcpServer.stop();
    endpoint.libDestroy();
    std::cout << "SIPCore destroyed" << std::endl;

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
            sendState(
                username,
                state,
                remote
            );

            if (state == "disconnected")
            {
                std::cout
                    << "CALL REMOVED: "
                    << username
                    << std::endl;

                calls.erase(username);
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

    auto call =
        std::make_shared<SIPCall>(
            *it->second
        );

    setupCall(
        username,
        call
    );

    pj::CallOpParam prm(true);

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


void SIPCore::sendState(
    const std::string& username,
    const std::string& state,
    const std::string& remote
)
{
    std::string json =
        "{"
        "\"username\":\"" + username + "\","
        "\"state\":\"" + state + "\"";

    if (!remote.empty())
    {
        json +=
            ",\"remote\":\"" + remote + "\"";
    }

    json += "}";

    tcpServer.sendMessage(json);

    std::cout
        << "[STATE] "
        << json
        << std::endl;
}


void SIPCore::handleTcpMessage(const std::string& msg)
{
    JsonCommand cmd =
    Json::parse(msg);

    if (!cmd.valid)
    {
        std::cout
            << "INVALID JSON"
            << std::endl;

        return;
    }

    if (cmd.command == "registration")
    {
        registerAccount(
            cmd.server,
            cmd.username,
            cmd.password
        );
    }

    if (cmd.command == "disconnect")
    {
        disconnectAccount(cmd.username);
    }

    if (cmd.command == "call")
    {
        makeCall(
            cmd.username,
            cmd.remote,
            cmd.server
        );
    }

    if (cmd.command == "answer")
    {
        answerCall(
            cmd.username
        );
    }

    if (cmd.command == "hangup")
    {
        hangupCall(
            cmd.username
        );
    }

}