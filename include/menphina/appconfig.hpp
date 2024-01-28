/* Copyright 2024 isaki */

#ifndef __MENPHINA_APP_CONFIG_HPP__
#define __MENPHINA_APP_CONFIG_HPP__

#include <string>
#include <ostream>

namespace menphina
{
    struct app_config_s
    {
        std::string penumbraDir;
        std::string xivLauncherConfigDir;
    };

    std::ostream& operator<<(std::ostream& os, const struct app_config_s& ac);
}

#endif