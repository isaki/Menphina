/* Copyright 2024 isaki */

#ifndef __MENPHINA_UTIL_HPP__
#define __MENPHINA_UTIL_HPP__

#include <string>
#include <string_view>
#include "glaze/exceptions/json_exceptions.hpp"

namespace menphina
{
    std::string path_basename(const std::string_view& pathstr);

    std::string get_user_home_directory();

    std::string path_join(const std::string& a, const std::string& b);

    template<typename T>
    inline void read_json_file(T& obj, const std::string& filename)
    {
        glz::ex::read_file_json(obj, filename, std::string {});
    }
}

#endif