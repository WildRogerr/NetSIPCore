/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#pragma once
#include <pjsua2.hpp>
#include <functional>
#include <memory>
#include <string>



class RTPAudioPlayer : public pj::AudioMediaPlayer
{
public:
    std::function<void()> finishedCallback;

protected:
    void onEof2() override;

};


class RTPPlayer
{
public:

    RTPPlayer();
    void play(
        pj::AudioMedia* callMedia,
        const std::string& wavFile,
        std::function<void()> finished
    );
    void stop();
    bool isPlaying() const;
    void setFinishedCallback(std::function<void()> cb);

private:

    std::unique_ptr<RTPAudioPlayer> player;
    std::function<void()> finishedCallback;
    bool playing = false;
    pj::AudioMedia* currentTarget = nullptr;

};