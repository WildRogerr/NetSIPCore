/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

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

    core.destroy();

    return 0;
    
}