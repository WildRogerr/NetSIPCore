/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include "Call.hpp"



SIPCall::SIPCall(pj::Account& acc,int call_id):Call(acc, call_id) {}


SIPCall::~SIPCall()
{
    std::cout
        << "Call destroyed"
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
        << "CALL STATE "
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

            pj::AudioMedia* audioMedia = nullptr;

            try
            {
                std::lock_guard<std::mutex> lock(mediaMutex);

                if (mediaActive)
                    continue;

                currentAudioMedia = static_cast<pj::AudioMedia*>(getMedia(i));

                audioMedia = currentAudioMedia;

                if (!audioMedia)
                    continue;

            }
            catch (pj::Error& err)
            {
                std::cout
                    << "Media get error: "
                    << err.info()
                    << std::endl;

                continue;
            }

            if (microphoneEnabled)
            {
                try
                {
                    adm.getCaptureDevMedia()
                        .startTransmit(*audioMedia);
                }
                catch (pj::Error &err)
                {
                    std::cout
                        << "Mic connect error: "
                        << err.info()
                        << std::endl;
                }
            }

            if (speakerEnabled)
            {
                try
                {
                    audioMedia->startTransmit(
                        adm.getPlaybackDevMedia()
                    );
                }
                catch (pj::Error &err)
                {
                    std::cout
                        << "Speaker connect error: "
                        << err.info()
                        << std::endl;
                }
            }

            {
                std::lock_guard<std::mutex> lock(mediaMutex);
                mediaActive = true;
            }

            std::cout
                << "Media connected"
                << std::endl;

        }

        // MEDIA DISCONNECTED

        else
        {

            {
                std::lock_guard<std::mutex> lock(mediaMutex);
                mediaActive = false;
                currentAudioMedia = nullptr;
            }
            
            std::cout
                << "Media disconnected"
                << std::endl;
        }
    }
}


int SIPCall::getCallId()
{
    return getInfo().id;
}


void SIPCall::setSpeakerState(bool state)
{

    pj::AudioMedia* audioMedia = nullptr;

    {
        std::lock_guard<std::mutex> lock(mediaMutex);

        if (speakerEnabled == state)
            return;

        speakerEnabled = state;

        if (!mediaActive)
            return;

        audioMedia = currentAudioMedia;

        if (!audioMedia)
            return;
    }

    auto& adm = pj::Endpoint::instance().audDevManager();

    try
    {
        if (speakerEnabled)
        {
            audioMedia->startTransmit(adm.getPlaybackDevMedia());
        }
        else
        {
            audioMedia->stopTransmit(adm.getPlaybackDevMedia());
        }
    }
    catch(...)
    {
    }
}


void SIPCall::setMicrophoneState(bool state)
{
    pj::AudioMedia* audioMedia = nullptr;

    {
        std::lock_guard<std::mutex> lock(mediaMutex);

        if (microphoneEnabled == state)
            return;

        microphoneEnabled = state;

        if (!mediaActive)
            return;
        
        audioMedia = currentAudioMedia;

        if (!audioMedia)
            return;
    }

    auto& adm = pj::Endpoint::instance().audDevManager();

    try
    {
        if (microphoneEnabled)
        {
            adm.getCaptureDevMedia().startTransmit(*audioMedia);
        }
        else
        {
            adm.getCaptureDevMedia().stopTransmit(*audioMedia);
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
    pj::AudioMedia* audioMedia = nullptr;

    {
        std::lock_guard<std::mutex> lock(mediaMutex);

        if (!mediaActive)
            return;

        audioMedia = currentAudioMedia;

        if (!audioMedia)
            return;

        if (path.empty())
            return;

        if (rtpPlayer.isPlaying())
        {
            std::cout << "Audio already playing" << std::endl;
            return;
        }
    }

    rtpPlayer.play(
        audioMedia,
        path,
        std::move(finished)
    );
}