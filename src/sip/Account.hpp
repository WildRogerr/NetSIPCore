/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#pragma once
#include <pjsua2.hpp>
#include <functional>
#include "Call.hpp"
#include <memory>
#include <iostream>



class SIPAccount : public pj::Account
{
    
    public:

        virtual void onRegState(pj::OnRegStateParam &prm) override;
        virtual void onIncomingCall(pj::OnIncomingCallParam &iprm) override;
        std::function<void(const std::string&, const std::string&, const std::string&)> stateCallback;
        std::function<void(std::shared_ptr<SIPCall>)> callCallback;
        void setStateCallback(std::function<void(const std::string&, const std::string&, const std::string&)> cb);
        void setCallCallback(std::function<void(std::shared_ptr<SIPCall>)> cb
);

};