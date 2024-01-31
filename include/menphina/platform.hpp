/* Copyright 2024 isaki */

#ifndef __MENPHINA_PLATFORM_HPP__
#define __MENPHINA_PLATFORM_HPP__

/*
    This header exists so I can hide all the macro hell involved in compiling
    for and supporting operation in different environments. As such, the
    functions may seem like an ecclectic collection of random things.
*/

#include <cstdint>
#include <string>
#include <string_view>

namespace menphina
{
    enum class Platform : uint8_t
    {
        Darwin = 0,
        Windows = 1,
        Linux = 2,
        WSL = 3
    };

    Platform get_current_platform();

    std::string path_basename(const std::string_view pathstr);
    std::string path_join(const std::string_view a, const std::string_view b);
    bool path_exists(const std::string_view path);

    const std::string & get_user_home_directory();

    const std::string & get_relative_launcher_config_dir();
}

#endif
