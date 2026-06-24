#pragma once
#include <pjsua2.hpp>



class SIPAccount : public pj::Account
{
    public:

    virtual void onRegState(pj::OnRegStateParam &prm) override;

};