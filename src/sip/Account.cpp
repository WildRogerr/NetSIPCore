/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include "Account.hpp"



void SIPAccount::onRegState(pj::OnRegStateParam &prm)
{
    pj::AccountInfo info = getInfo();

    std::cout
        << "Register state: "
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


void SIPAccount::onIncomingCall(pj::OnIncomingCallParam& iprm)
{
    auto call =
        std::make_shared<SIPCall>(
            *this,
            iprm.callId
        );

    bool accepted = false;

    if (callCallback)
    {
        accepted = callCallback(call);
    }

    try
    {
        pj::CallOpParam prm;

        if (accepted)
        {
            prm.statusCode = PJSIP_SC_RINGING;
        }
        else
        {
            prm.statusCode = PJSIP_SC_BUSY_HERE;
        }

        call->answer(prm);
    }
    catch (pj::Error& err)
    {
        std::cout
            << "Incoming call response error: "
            << err.info()
            << std::endl;
    }
}


void SIPAccount::setStateCallback(std::function<void(const std::string&, const std::string&, const std::string&)> cb)
{
    stateCallback = cb;
}


void SIPAccount::setCallCallback(std::function<bool(std::shared_ptr<SIPCall>)> cb)
{
    callCallback = cb;
}