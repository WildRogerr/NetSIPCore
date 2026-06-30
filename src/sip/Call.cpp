#include "Call.hpp"
#include <iostream>



SIPCall::SIPCall(pj::Account& acc,int call_id):Call(acc, call_id) {}


SIPCall::~SIPCall()
{
    std::cout
        << "CALL DESTROYED"
        << std::endl;
}


void SIPCall::onCallState(pj::OnCallStateParam &prm)
{
    pj::CallInfo info = getInfo();

    std::string state;

    switch (info.state)
    {
        case PJSIP_INV_STATE_CALLING:
            state = "calling";
            break;

        case PJSIP_INV_STATE_INCOMING:
            state = "incoming";
            break;

        case PJSIP_INV_STATE_EARLY:
            state = "ringing";
            break;

        case PJSIP_INV_STATE_CONNECTING:
            state = "connecting";
            break;

        case PJSIP_INV_STATE_CONFIRMED:
            state = "confirmed";
            break;

        case PJSIP_INV_STATE_DISCONNECTED:
            state = "disconnected";
            break;

        default:
            state = "unknown";
            break;
    }

    std::cout
        << "[CALL STATE] "
        << state
        << " | "
        << info.lastStatusCode
        << " "
        << info.lastReason
        << std::endl;

    if (stateCallback)
    {
        stateCallback(
            info.localUri,
            state,
            info.remoteUri
        );
    }
}


int SIPCall::getCallId()
{
    return getInfo().id;
}