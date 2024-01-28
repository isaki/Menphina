/* Copyright 2024 isaki */

#include "menphina/util.hpp"

namespace menphina
{

    std::string path_basename(const std::string_view& pathstr)
    {
        const std::filesystem::path p(pathstr);
        return p.filename().u8string();
    }

}