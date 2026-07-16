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

    try
    {   
        finishedCallback = std::move(finished);
        player.reset();
        player = std::make_unique<RTPAudioPlayer>();

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
            player.reset();
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
    std::cout << "EOF CALLBACK" << std::endl; ////////

    if (finishedCallback)
        finishedCallback();
}