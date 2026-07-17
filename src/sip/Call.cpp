#include "Call.hpp"
#include <pjsip/sip_event.h>
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
    std::lock_guard<std::mutex> lock(mediaMutex);
    
    pj::CallInfo info = getInfo();

    auto& adm = pj::Endpoint::instance().audDevManager();

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
            if (mediaConnected)
                continue;

            try
            {
                currentAudioMedia = static_cast<pj::AudioMedia*>(getMedia(i));

                if (!currentAudioMedia)
                {
                    return;
                }

                if (microphoneEnabled)
                {
                    adm.getCaptureDevMedia()
                        .startTransmit(*currentAudioMedia);
                }

                if (speakerEnabled)
                {
                    currentAudioMedia->startTransmit(
                        adm.getPlaybackDevMedia()
                    );
                }

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
            if (!mediaConnected)
                continue;

            try
            {
                auto& adm =
                    pj::Endpoint::instance().audDevManager();
                
                if (microphoneEnabled)
                {
                    adm.getCaptureDevMedia()
                        .stopTransmit(*currentAudioMedia);
                }

                if (speakerEnabled)
                {
                    currentAudioMedia->stopTransmit(
                        adm.getPlaybackDevMedia()
                    );
                }

                mediaConnected = false;
                currentAudioMedia = nullptr;

                std::cout
                    << "[MEDIA DISCONNECTED]"
                    << std::endl;
            }
            catch (pj::Error& err)
            {
                std::cout
                    << "MEDIA DISCONNECT ERROR: "
                    << err.info()
                    << std::endl;

                mediaConnected = false;
                currentAudioMedia = nullptr;
            }
        }
    }
}


int SIPCall::getCallId()
{
    return getInfo().id;
}


void SIPCall::setSpeakerState(bool state)
{
    std::lock_guard<std::mutex> lock(mediaMutex);

    speakerEnabled = state;

    if (!mediaConnected)
        return;

    if (!currentAudioMedia)
        return;

    auto& adm = pj::Endpoint::instance().audDevManager();

    try
    {
        if (speakerEnabled)
        {
            currentAudioMedia->startTransmit(adm.getPlaybackDevMedia());
        }
        else
        {
            currentAudioMedia->stopTransmit(adm.getPlaybackDevMedia());
        }
    }
    catch(...)
    {
    }
}


void SIPCall::setMicrophoneState(bool state)
{
    std::lock_guard<std::mutex> lock(mediaMutex);

    if (microphoneEnabled == state)
        return;

    microphoneEnabled = state;

    if (!mediaConnected)
        return;

    if (!currentAudioMedia)
        return;

    auto& adm = pj::Endpoint::instance().audDevManager();

    try
    {
        if (microphoneEnabled)
        {
            adm.getCaptureDevMedia().startTransmit(*currentAudioMedia);
        }
        else
        {
            adm.getCaptureDevMedia().stopTransmit(*currentAudioMedia);
        }
    }
    catch(...)
    {
    }
}


std::string SIPCall::getRemoteUri()
{
    return getInfo().remoteUri;
}


void SIPCall::playAudio(
    const std::string& path,
    std::function<void()> finished
)
{

    std::lock_guard<std::mutex> lock(mediaMutex);

    if (!mediaConnected)
        return;

    if (!currentAudioMedia)
        return;

    if (path.empty())
        return;

    if (rtpPlayer.isPlaying())
    {
        std::cout << "Audio already playing" << std::endl;
        return;
    }

    rtpPlayer.play(
        currentAudioMedia,
        path,
        std::move(finished)
    );
}