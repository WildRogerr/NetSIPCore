#pragma once
#include <pjsua2.hpp>
#include <functional>



class SIPCall : public pj::Call
{
    public:

        SIPCall(pj::Account& acc, int call_id = PJSUA_INVALID_ID);
        ~SIPCall();
        virtual void onCallState(pj::OnCallStateParam &prm) override;
        std::function < void(const std::string&, const std::string&, const std::string&) > stateCallback;
        int getCallId();

};