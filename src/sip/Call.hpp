#pragma once
#include <pjsua2.hpp>
#include <functional>
#include <iostream>



class SIPCall : public pj::Call
{
    public:

        SIPCall(pj::Account& acc, int call_id = PJSUA_INVALID_ID);
        virtual void onCallState(pj::OnCallStateParam &prm) override;
        std::function < void(const std::string&, const std::string&, const std::string&) > stateCallback;
        int getCallId();
        ~SIPCall()
        {
            std::cout
                << "CALL DESTROYED"
                << std::endl;
        }

};