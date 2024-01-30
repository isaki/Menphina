/* Copyright 2024 isaki */

#ifndef __MENPHINA_PLATFORM_HPP__
#define __MENPHINA_PLATFORM_HPP__

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

    const std::string & get_user_home_directory();
}

#endif
