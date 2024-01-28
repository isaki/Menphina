/* Copyright 2024 isaki */

#include "menphina/appconfig.hpp"

std::ostream& menphina::operator<<(std::ostream& os, const struct app_config_s& ac)
{
    os << std::endl
        << "- penumbraDir: " << ac.penumbraDir
        << std::endl
        << "- xivLauncherConfigDir: " << ac.xivLauncherConfigDir
        << std::endl;
    
    return os;
}
