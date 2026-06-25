#pragma once
#include <string>



struct JsonCommand
{
    std::string command;
    std::string server;
    std::string username;
    std::string password;
    std::string calling_number;
    std::string current_state;
    std::string audio_state;
    bool valid = false;

};



class Json
{
public:

    static JsonCommand parse(const std::string& raw);

};