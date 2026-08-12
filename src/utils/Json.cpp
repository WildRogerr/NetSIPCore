/*
 * NetSIPCore
 * Copyright (C) 2026 WildRogerr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

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
        json.find(':', keyPos + search.length());

    if (colon == std::string::npos)
    {
        return "";
    }

    size_t firstQuote =
        json.find('"', colon + 1);

    if (firstQuote == std::string::npos)
    {
        return "";
    }

    std::string result;

    bool escaped = false;

    for (size_t i = firstQuote + 1; i < json.length(); ++i)
    {
        char c = json[i];

        if (escaped)
        {
            switch (c)
            {
                case '"':
                    result += '"';
                    break;

                case '\\':
                    result += '\\';
                    break;

                case 'n':
                    result += '\n';
                    break;

                case 'r':
                    result += '\r';
                    break;

                case 't':
                    result += '\t';
                    break;

                default:
                    result += c;
                    break;
            }

            escaped = false;
            continue;
        }

        if (c == '\\')
        {
            escaped = true;
            continue;
        }

        if (c == '"')
        {
            return result;
        }

        result += c;
    }

    return "";
}



JsonCommand Json::parse(const std::string& raw)
{

    JsonCommand cmd;

    cmd.command =
        extract(raw, "command");

    cmd.server =
        extract(raw, "server");

    cmd.proxy =
        extract(raw, "proxy");

    cmd.username =
        extract(raw, "username");

    cmd.auth_username =
        extract(raw, "auth_username");

    cmd.password =
        extract(raw, "password");

    cmd.remote =
        extract(raw, "remote");

    cmd.current_state =
        extract(raw, "current_state");

    cmd.audio_state =
        extract(raw, "audio_state");

    cmd.device =
        extract(raw, "device");

    cmd.audio_path =
        extract(raw, "audio_path");

    if (!cmd.command.empty())
    {
        cmd.valid = true;
    }

    return cmd;

}