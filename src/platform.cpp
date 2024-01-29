/* Copyright 2024 isaki */

#include <string>
#include <string_view>
#include <cstdlib>
#include <filesystem>
#include <exception>

#if defined ( __linux__ )
#include <unistd.h>
#include <cerrno>
#include <fstream>
#endif

#include "menphina/platform.hpp"
#include "menphina/m_exception.hpp"

namespace
{
#if defined ( _WIN32 )
    // While this is not targetting Windows for the first release, windows suppose is planned.
    const std::string HOME_VAR_NAME { "USERPROFILE" };
#else
    const std::string HOME_VAR_NAME { "HOME" };
#endif

#if defined ( __linux__ )
    const std::string PROC_VERSION { "/proc/version" };
    const std::string WSL_VERSION_STRING { "microsoft" };

    const std::string WSL_HOME_VAR_NAME { "USERPROFILE" };

    menphina::Platform _get_linux_platform()
    {
        std::ifstream is;
        is.exceptions(std::ios::failbit | std::ios::badbit);
        is.open(PROC_VERSION, std::ios::ate);

        auto size = is.tellg();

        // Allocate memory, make sure its nulled out
        std::string str(size, '\0');

        // Rewind the read.
        is.seekg(0);

        // Read into our memory buffer
        is.read(str.data(), size);

        // While this method's exit will do this, no harm in doing it here.
        is.close();

        return (str.find(WSL_VERSION_STRING) != std::string::npos) ? menphina::Platform::WSL : menphina::Platform::Linux;
    }

    // std::string _get_win_env_r(const std::string& varname)
    // {
    //     int fd[2];
    //     if (pipe(fd) != 0) {
    //         const int err = errno;
    //         throw menphina::ex::errno_exception("Unable to create pipe", err);
    //     }

    //     return varname;
    // }

    std::string _get_wsl_home()
    {
        return {};
    }
#endif

    std::string _get_home()
    {
        // Documentation for getpwnam/getpwuid say to use HOME, so we use that.
#if defined ( __linux__ )
        const menphina::Platform p = menphina::get_current_platform();
        if (p == menphina::Platform::WSL)
        {
            return _get_wsl_home();
        }
#endif

        const char * home = std::getenv(HOME_VAR_NAME.c_str());

        if (home == nullptr)
        {
            throw menphina::str_exception("Unable to read env " + HOME_VAR_NAME);
        }

        std::string ret { home };
        return ret;   
    }
}

menphina::Platform menphina::get_current_platform()
{
#if defined ( __linux__ )
    static menphina::Platform p = _get_linux_platform();
    return p;
#elif defined ( __APPLE__ )
    return menphina::Platform::Darwin;
#elif defined ( _WIN32 )
    return menphina::Platform::Windows;
#else
#error "Unsupported platform for menphina::get_current_platform()"
#endif
}

std::string menphina::path_basename(const std::string_view& pathstr)
{
    const std::filesystem::path p(pathstr);
    return p.filename().string();
}

const std::string & menphina::get_user_home_directory()
{
    static const std::string h = _get_home();
    return h;
}

std::string menphina::path_join(const std::string& a, const std::string& b)
{
    std::filesystem::path ret(a);
    ret /= b;
    return ret;
}