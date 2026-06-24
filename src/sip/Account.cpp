#include "Account.hpp"
#include <iostream>



void SIPAccount::onRegState(pj::OnRegStateParam &prm)
{
    
    pj::AccountInfo info = getInfo();

    std::cout
        << "REGISTER STATE: "
        << info.regStatus
        << " "
        << info.regStatusText
        << std::endl;

}