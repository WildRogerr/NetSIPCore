#pragma once
#include <string>



struct JsonCommand
{
    std::string command;
    std::string server;
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