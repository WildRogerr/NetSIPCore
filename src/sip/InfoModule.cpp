#include "InfoModule.h"



pjsip_module InfoModule::module =
{
    nullptr,                         // prev
    nullptr,                         // next
    { (char*)"mod-info", 8 },        // name
    -1,                              // id
    PJSIP_MOD_PRIORITY_UA_PROXY_LAYER - 1,  // priority

    nullptr, // load
    nullptr, // start
    nullptr, // stop
    nullptr, // unload

    &InfoModule::onRxRequest,

    nullptr, // on_rx_response
    nullptr, // on_tx_request
    nullptr, // on_tx_response
    nullptr, // on_tsx_state
};


void InfoModule::init()
{  
    pjsip_endpt_register_module(
        pjsua_get_pjsip_endpt(),
        &module
    );
}


pj_bool_t InfoModule::onRxRequest(pjsip_rx_data *rdata)
{
    auto *msg = rdata->msg_info.msg;

    if (msg->type != PJSIP_REQUEST_MSG)
        return PJ_FALSE;

    if (pj_stricmp2(
            &msg->line.req.method.name,
            "INFO") == 0)
    {

        pjsip_endpt_respond_stateless(
            pjsua_get_pjsip_endpt(),
            rdata,
            200,
            NULL,
            NULL,
            NULL
        );

        return PJ_TRUE;
    }

    return PJ_FALSE;
}