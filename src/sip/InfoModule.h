/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#pragma once

extern "C" {
#include <pjsua-lib/pjsua.h>
}

class InfoModule
{

public:
    static void init();

private:
    static pj_bool_t onRxRequest(pjsip_rx_data *rdata);
    static pjsip_module module;
    
};