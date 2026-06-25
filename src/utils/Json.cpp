#include "Json.hpp"



static std::string extract(const std::string& json, const std::string& key)
{
    
    std::string search =
        "\"" + key + "\"";

    size_t keyPos =
        json.find(search);

    if (keyPos == std::string::npos)
    {
        return "";
    }

    size_t colon =
        json.find(':', keyPos);

    size_t firstQuote =
        json.find('"', colon + 1);

    size_t secondQuote =
        json.find('"', firstQuote + 1);

    if (
        firstQuote == std::string::npos ||
        secondQuote == std::string::npos
    )
    {
        return "";
    }

    return json.substr(
        firstQuote + 1,
        secondQuote - firstQuote - 1
    );

}



JsonCommand Json::parse(const std::string& raw)
{

    JsonCommand cmd;

    cmd.command =
        extract(raw, "command");

    cmd.server =
        extract(raw, "server");

    cmd.username =
        extract(raw, "username");

    cmd.password =
        extract(raw, "password");

    cmd.calling_number =
        extract(raw, "calling_number");

    cmd.current_state =
        extract(raw, "current_state");

    cmd.audio_state =
        extract(raw, "audio_state");

    if (!cmd.command.empty())
    {
        cmd.valid = true;
    }

    return cmd;

}