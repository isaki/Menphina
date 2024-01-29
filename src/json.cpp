/* Copyright 2024 isaki */

#include <string>
#include <ostream>
#include "menphina/json.hpp"
#include "glaze/exceptions/json_exceptions.hpp"

namespace
{
    template<typename T>
    inline void _read_json_file(T& obj, const std::string& jsonFile)
    {
        glz::ex::read_file_json(obj, jsonFile, std::string {});
    }

    template<typename T>
    inline void _write_json_file(const T& obj, const std::string& jsonFile)
    {
        glz::ex::write_file_json(obj, jsonFile, std::string {});
    }
}

std::ostream& menphina::operator<<(std::ostream& os, const struct app_config_s& ac)
{
    os << std::endl
        << "- penumbraDir: " << ac.penumbraDir
        << std::endl
        << "- xivLauncherConfigDir: " << ac.xivLauncherConfigDir
        << std::endl;
    
    return os;
}

void menphina::read_json_file(struct app_config_s& obj, const std::string& jsonFile)
{
    _read_json_file(obj, jsonFile);
}

void menphina::write_json_file(const struct app_config_s& obj, const std::string& jsonFile)
{
    _write_json_file(obj, jsonFile);
}