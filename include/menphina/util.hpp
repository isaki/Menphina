/* Copyright 2024 isaki */

#ifndef __MENPHINA_UTIL_HPP__
#define __MENPHINA_UTIL_HPP__

#include <filesystem>
#include <string_view>

namespace menphina
{
    std::string path_basename(const std::string_view& pathstr);
}

#endif