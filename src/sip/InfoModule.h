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