/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include "RTP.hpp"
#include <iostream>



RTPPlayer::RTPPlayer()
{
    player = std::make_unique<RTPAudioPlayer>();

    player->finishedCallback =
    [this]()
    {
        playing = false;
        currentTarget = nullptr;

        if (finishedCallback)
            finishedCallback();
    };
}


void RTPPlayer::setFinishedCallback(std::function<void()> cb)
{
    finishedCallback = std::move(cb);
}


bool RTPPlayer::isPlaying() const
{
    return playing;
}


void RTPPlayer::play(
    pj::AudioMedia* callMedia,
    const std::string& wavFile,
    std::function<void()> finished
)
{
    if (!callMedia)
        return;

    if (playing)
        stop();

    if (player)
        player.reset();

    try
    {   

        player = std::make_unique<RTPAudioPlayer>();

        finishedCallback = std::move(finished);

        player->finishedCallback =
        [this]()
        {
            playing = false;
            currentTarget = nullptr;

            if (finishedCallback)
                finishedCallback();
        };

        player->createPlayer(
            wavFile,
            PJMEDIA_FILE_NO_LOOP
        );

        currentTarget = callMedia;
        player->startTransmit(*currentTarget);

        playing = true;

        std::cout
            << "RTP PLAY STARTED: "
            << wavFile
            << std::endl;
    }
    catch (pj::Error& err)
    {
        std::cout
            << "RTP PLAY ERROR: "
            << err.info()
            << std::endl;

        playing = false;
    }
}


void RTPPlayer::stop()
{
    if (!playing)
        return;

    try
    {
        if (currentTarget)
        {
            player->stopTransmit(*currentTarget);
            currentTarget = nullptr;
        }
        playing = false;
    }
    catch (...)
    {
        playing = false;
    }
}


void RTPAudioPlayer::onEof2()
{
    if (finishedCallback)
    {
        finishedCallback();
    }
}