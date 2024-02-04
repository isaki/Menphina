/* Copyright 2024 isaki */

#ifndef __MENPHINA_APP_CONFIG_HPP__
#define __MENPHINA_APP_CONFIG_HPP__

#include <string>
#include <ostream>
#include <cstdint>
#include "glaze/json/json_t.hpp"

// Note the use of Pascal case is to match what Dalamud plugins typically use
// so as to be familiar to the end user.
namespace menphina
{
    /* PENUMBRA */

    struct penumbra_config_t
    {
        std::string ModDirectory;
    };

    void read_json_file(penumbra_config_t& obj, const std::string& jsonFile);


    /* GENERIC ACCESS */
    /* This is required to support not having to know every single field in every single json */ 
    void read_generic_json_file(glz::json_t& obj, const std::string& jsonFile);
    void write_generic_json_file(const glz::json_t& obj, const std::string& jsonFile);
}

#endif
