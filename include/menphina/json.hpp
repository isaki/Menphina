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
    // This is merely an abstraction.
    using json_t = glz::json_t;

    /* PENUMBRA */

    struct penumbra_config_t
    {
        std::string ModDirectory;
        std::string ExportDirectory;
    };

    void read_json_file(penumbra_config_t& obj, const std::string& jsonFile);

    /* MARE SYNCHRONOS */

    struct mare_config_t
    {
        std::string CacheFolder;
    };

    void read_json_file(mare_config_t& obj, const std::string& jsonFile);

    /* MENPHINA */

    struct app_config_t
    {
        /* Common JSON constructs with plugins */
        penumbra_config_t Penumbra;
        mare_config_t MareSynchronos;

        /* Unique settings for this application */
        struct
        {
            std::string ConfigDirectory;
        } XIVLauncher;
    };

    void read_json_file(app_config_t& obj, const std::string& jsonFile);
    void write_json_file(const app_config_t& obj, const std::string& jsonFile);

    /* GENERIC ACCESS */
    /* This is required to support not having to know every single field in every single json */

}

#endif
