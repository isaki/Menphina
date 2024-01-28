/* Copyright 2024 isaki */

#include <cstdlib>
#include <cstdarg>
#include <cstddef>
#include <filesystem>

#include "menphina/util.hpp"

namespace
{
#if defined ( _WIN32 )
    // While this is not targetting Windows for the first release, windows suppose is planned.
    const std::string HOME_VAR_NAME { "USERPROFILE" };
#else
    const std::string HOME_VAR_NAME { "HOME" };
#endif
}

std::string menphina::path_basename(const std::string_view& pathstr)
{
    const std::filesystem::path p(pathstr);
    return p.filename().string();
}

std::string menphina::get_user_home_directory()
{
    // Documentation for getpwnam/getpwuid say to use HOME, so we use that.
    const char * home = std::getenv(HOME_VAR_NAME.c_str());

    if (home == nullptr)
    {
        return {};
    }

    std::string ret { home };
    return ret;
}

std::string menphina::path_join(const std::string& a, const std::string& b)
{
    std::filesystem::path ret(a);
    ret /= b;
    return ret;
}

