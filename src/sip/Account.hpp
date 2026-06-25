#pragma once
#include <pjsua2.hpp>
#include <functional>



class SIPAccount : public pj::Account
{
    public:

        virtual void onRegState(pj::OnRegStateParam &prm) override;
        std::function < void(const std::string&, const std::string&) > regStateCallback;
        void setRegStateCallback(std::function < void(const std::string&, const std::string&) > cb);
};