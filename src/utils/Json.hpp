/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#pragma once
#include <string>



struct JsonCommand
{
    std::string command;
    std::string server;
    std::string proxy;
    std::string username;
    std::string password;
    std::string remote;
    std::string current_state;
    std::string audio_state;
    std::string device;
    std::string audio_path;
    bool valid = false;
    
};



class Json
{
public:

    static JsonCommand parse(const std::string& raw);

};