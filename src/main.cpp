#include <iostream>
#include <pjsua2.hpp>
#include "SIPCore.hpp"



int main() {

    SIPCore core;

    try {
        core.init();
        // core.registerAccount(
        //     "192.168.124.96",
        //     "343448789",
        //     "1111"
        // );
        core.run();
        core.destroy();

    } catch (pj::Error &e) {
        std::cout << e.info() << std::endl;
    }

    return 0;
}