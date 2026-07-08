#include "Call.hpp"
#include <iostream>



SIPCall::SIPCall(pj::Account& acc,int call_id):Call(acc, call_id) {}


SIPCall::~SIPCall()
{
    std::cout
        << "CALL DESTROYED"
        << std::endl;
}


void SIPCall::onCallState(pj::OnCallStateParam &prm)
{
    pj::CallInfo info = getInfo();

    std::string state;

    switch (info.state)
    {
        case PJSIP_INV_STATE_CALLING:
            state = "calling";
            break;

        case PJSIP_INV_STATE_INCOMING:
            state = "incoming";
            break;

        case PJSIP_INV_STATE_EARLY:
            state = "ringing";
            break;

        case PJSIP_INV_STATE_CONNECTING:
            state = "connecting";
            break;

        case PJSIP_INV_STATE_CONFIRMED:
            state = "confirmed";
            break;

        case PJSIP_INV_STATE_DISCONNECTED:
            state = "disconnected";
            break;

        default:
            state = "unknown";
            break;
    }

    std::cout
        << "[CALL STATE] "
        << state
        << " | "
        << info.lastStatusCode
        << " "
        << info.lastReason
        << std::endl;

    if (info.state == PJSIP_INV_STATE_DISCONNECTED)
    {   
        try
        {
            if (mediaConnected && currentAudioMedia)
            {
                auto& adm =
                    pj::Endpoint::instance().audDevManager();

                adm.getCaptureDevMedia()
                    .stopTransmit(*currentAudioMedia);

                currentAudioMedia->stopTransmit(
                    adm.getPlaybackDevMedia()
                );

                std::cout
                    << "[MEDIA DISCONNECTED]"
                    << std::endl;

                mediaConnected = false;
                currentAudioMedia = nullptr;
            }
        }
        catch (pj::Error& err)
        {
            std::cout
                << "MEDIA DISCONNECT ERROR: "
                << err.info()
                << std::endl;
        }
    }

    if (stateCallback)
    {
        stateCallback(
            info.localUri,
            state,
            info.remoteUri
        );
    }
}


void SIPCall::onCallMediaState(pj::OnCallMediaStateParam &prm)
{
    pj::CallInfo info = getInfo();

    auto& adm =
        pj::Endpoint::instance().audDevManager();

    for (unsigned i = 0; i < info.media.size(); i++)
    {
        auto& media = info.media[i];

        if (media.type != PJMEDIA_TYPE_AUDIO)
        {
            continue;
        }

        // MEDIA ACTIVE

        if (media.status == PJSUA_CALL_MEDIA_ACTIVE)
        {
            try
            {
                currentAudioMedia = static_cast<pj::AudioMedia*>(getMedia(i));

                if (!currentAudioMedia)
                {
                    return;
                }

                adm.getCaptureDevMedia()
                    .startTransmit(*currentAudioMedia);

                currentAudioMedia->startTransmit(
                    adm.getPlaybackDevMedia()
                );

                mediaConnected = true;

                std::cout
                    << "[MEDIA CONNECTED]"
                    << std::endl;
            }
            catch (pj::Error& err)
            {
                std::cout
                    << "MEDIA CONNECT ERROR: "
                    << err.info()
                    << std::endl;
            }
        }

        // MEDIA DISCONNECTED

        else
        {
            try
            {
                if (mediaConnected && currentAudioMedia)
                {
                    adm.getCaptureDevMedia()
                        .stopTransmit(*currentAudioMedia);

                    currentAudioMedia->stopTransmit(
                        adm.getPlaybackDevMedia()
                    );

                    std::cout
                        << "[MEDIA DISCONNECTED]"
                        << std::endl;

                    mediaConnected = false;
                    currentAudioMedia = nullptr;
                }
            }
            catch (pj::Error& err)
            {
                std::cout
                    << "MEDIA DISCONNECT ERROR: "
                    << err.info()
                    << std::endl;
            }
        }
    }
}


int SIPCall::getCallId()
{
    return getInfo().id;
}