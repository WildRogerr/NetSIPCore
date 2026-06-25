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

    if (regStateCallback)
    {
        std::string state = "failed";

        if (prm.code == 200)
        {
            state = "registered";
        }

        regStateCallback(
            info.uri,
            state
        );
    }
}


void SIPAccount::setRegStateCallback(std::function<void(const std::string&, const std::string&) > cb)
{
    regStateCallback = cb;
}