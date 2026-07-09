#include <iostream>
#include <pjsua2.hpp>
#include "SIPCore.hpp"



int main() {

    SIPCore core;

    try {
        core.init();
        core.run();
    } catch (pj::Error &e) {
        std::cout << e.info() << std::endl;
    }

    return 0;
    
}