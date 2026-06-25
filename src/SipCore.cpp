#include "SIPCore.hpp"
#include <iostream>



SIPCore::SIPCore() {
}


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
        std::cout << "ERROR: SIPCore not initialized" << std::endl;
        return;
    }

    account = std::make_unique<SIPAccount>();

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
        [this](std::shared_ptr<SIPCall> call)
        {
            setupCall(call);
        }
    );

    try
    {
    pj::AccountConfig config;

        config.idUri =
            "sip:" + username + "@" + server;

        config.regConfig.registrarUri =
            "sip:" + server;

        pj::AuthCredInfo cred(
            "digest",
            "*",
            username,
            0,
            password
        );

        config.sipConfig.authCreds.push_back(cred);

        account->create(config);

        std::cout << "Register request sent" << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "PJSIP ERROR: "
            << err.info()
            << std::endl;
    }

}


void SIPCore::setupCall(std::shared_ptr<SIPCall > call)
{
    int callId = call->getCallId();

    calls[callId] = call;

    call->stateCallback =
        [this, callId](
            const std::string& local,
            const std::string& state,
            const std::string& remote
        )
        {
            std::string username =
                local.substr(
                    4,
                    local.find('@') - 4
                );

            sendState(
                username,
                state,
                remote
            );

            if (state == "disconnected")
            {
                std::cout
                    << "CALL REMOVED: "
                    << callId
                    << std::endl;

                calls.erase(callId);
            }
        };

    std::cout
        << "CALL STORED: "
        << callId
        << std::endl;
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

    JsonCommand cmd = Json::parse(msg);

    if (!cmd.valid)
    {
        std::cout
            << "INVALID JSON"
            << std::endl;

        return;
    }

    if (cmd.command == "registration")
    {
        std::cout
            << "REGISTER COMMAND"
            << std::endl;

        registerAccount(
            cmd.server,
            cmd.username,
            cmd.password
        );
    }

}