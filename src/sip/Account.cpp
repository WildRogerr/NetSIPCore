#include "Account.hpp"
#include <iostream>



void SIPAccount::onRegState(pj::OnRegStateParam &prm)
{
    pj::AccountInfo info = getInfo();

    std::cout
        << "REGISTER STATE: "
        << prm.code
        << " "
        << prm.reason
        << std::endl;

    if (stateCallback)
    {
        std::string state = "failed";

        if (prm.code == 200)
        {
            state = "registered";
        }

        std::string username = info.uri.substr(4, info.uri.find('@') - 4);

        stateCallback(
            username,
            state,
            ""
        );
    }
}


void SIPAccount::onIncomingCall(pj::OnIncomingCallParam &iprm)
{
    std::shared_ptr<SIPCall> call = std::make_shared<SIPCall>(*this, iprm.callId);

    if (callCallback)
    {
        callCallback(call);
    }
}


void SIPAccount::setStateCallback(std::function < void(const std::string&, const std::string&, const std::string&) > cb)
{
    stateCallback = cb;
}

void SIPAccount::setCallCallback(std::function < void(std::shared_ptr<SIPCall >) > cb)
{
    callCallback = cb;
}