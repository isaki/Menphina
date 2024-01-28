/* Copyright 2024 isaki */

#ifndef __MENPHINA_UTIL_HPP__
#define __MENPHINA_UTIL_HPP__

#include <string>
#include <string_view>

namespace menphina
{
    std::string path_basename(const std::string_view& pathstr);

    std::string get_user_home_directory();

    std::string path_join(const std::string& a, const std::string& b);
}

#endif