/* Copyright 2024 isaki */

#include <string>
#include <string_view>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>

#if defined ( __linux__ )
#include <unistd.h>
#include <cerrno>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <memory>
#include <cctype>
#endif

#include "menphina/m_exception.hpp"
#include "menphina/platform.hpp"

namespace
{

    // Dalamud/XIV on Mac/XIVLauncher Default Path
#if defined ( _WIN32 )
    const std::string LAUNCHER_PATH = "AppData\\Roaming\\XIVLauncher";
#elif defined ( __APPLE__ )
    const std::string LAUNCHER_PATH = "Library/Application Support/XIV on Mac";
#elif defined ( __linux__ )
    const std::string WSL_LAUNCHER_PATH = "AppData/Roaming/XIVLauncher";
    const std::string LINUX_LAUNCHER_PATH = ".xlcore";
#else
#error "Unsupported platform; no launcher path defined."
#endif

    // Sub directory for plugins.
    //const std::string PLUGIN_CONFIG_DIR_NAME = "pluginConfigs";

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

    const std::string WSL_WIN_PATH_START { "/mnt" };
    const size_t WSL_PATH_START_MIN_LENGTH = WSL_WIN_PATH_START.size() + 2;

    inline constexpr char WINDOWS_PATH_DELIM = '\\';
    inline constexpr char GENERIC_PATH_DELIM = '/';

    // ... 32,767... for an environment variable... + 2 for CRLF.
    inline constexpr size_t WIN_LINE_END_LENGTH = 2;
    inline constexpr size_t WIN_ENV_DATA_LENGTH = 32767;
    inline constexpr size_t WIN_ENV_READ_BUFFER_LENGTH = WIN_ENV_DATA_LENGTH + WIN_LINE_END_LENGTH;
    
    // C++ likes RAII
    class IOPipe final
    {
        public:
            IOPipe()
            {
                if (pipe(m_fd) != 0)
                {
                    const int err = errno; 
                    throw menphina::errno_exception("Unable to create pipe", err);
                }
            }

            ~IOPipe()
            {
                closeIndex(0);
                closeIndex(1);
            }

            inline int writer()
            {
                closeIndex(0);
                return m_fd[1];
            }

            inline int reader()
            {
                closeIndex(1);
                return m_fd[0];
            }
    
        private:
            void closeIndex(const size_t i) {
                if (m_fd[i] != -1)
                {
                    close(m_fd[i]);
                    m_fd[i] = -1;
                }
            }

            int m_fd[2];
    };

    menphina::Platform _get_linux_platform()
    {
        std::ifstream in(PROC_VERSION);
        if (!in)
        {
            throw menphina::file_open_exception(PROC_VERSION, true);
        }

        std::string line;
        if (!std::getline(in, line))
        {
            throw std::runtime_error("failure to read data from file: " + PROC_VERSION);
        }

        return (line.find(WSL_VERSION_STRING) != std::string::npos) ? menphina::Platform::WSL : menphina::Platform::Linux;
    }

    std::string _get_win_env(const std::string& v)
    {
        IOPipe xpipe;

        const pid_t pid = fork();
        if (pid == -1)
        {
            const int err = errno;
            throw menphina::errno_exception("Failed to fork child process", err);
        }

        if (pid == 0)
        {
            // We are the child.
            const int fd = xpipe.writer();
            int check = dup2(fd, 1);
            if (check == -1)
            {
                std::exit(errno);
            }

            FILE * devnull = fopen("/dev/null", "w");
            if (!devnull)
            {
                std::exit(errno);
            }

            check = dup2(fileno(devnull), 2);

            if (check == -1)
            {
                std::exit(errno);
            }

            // Construct null terminated argument.
            const size_t wsize = v.size() + 3;
            char * winenv = new char[wsize];
            winenv[0] = '%';
            memcpy(winenv + 1, v.c_str(), v.size());
            winenv[wsize - 2] = '%';
            winenv[wsize - 1] = '\0';

            execl("/mnt/c/Windows/system32/cmd.exe", "/c", "echo", winenv, nullptr);
            std::exit(errno);
        }


        // We are the parent.
        // Note: We don't have to close xpipe; it will take care of that in its
        // destructor invocation.
        const int fd = xpipe.reader();

        // We might as well do this on the heap, 32k on the stack seems... not great.
        // We use unique_ptr so we don't have to think about delete/free in the
        // various places this function can exit, via exception or success.

        // We don't need to allocate the null, we are forcing it to truncate.
        auto buffer = std::unique_ptr<char[]>(new char[WIN_ENV_READ_BUFFER_LENGTH]);
        char * ptr = buffer.get();
        size_t total_read = 0;
        size_t last_read = 0;

        do
        {
            last_read = read(fd, ptr, WIN_ENV_READ_BUFFER_LENGTH - total_read);

            if (last_read == 0)
            {
                break;
            }

            total_read += last_read;
            ptr += last_read;
        } while (total_read < WIN_ENV_READ_BUFFER_LENGTH);

        // This shouldn't happen unless our assumptions break, which, with WSL, you never know.
        if (last_read != 0)
        {
            // Drain down the buffer
            do
            {
                last_read = read(fd, buffer.get(), WIN_ENV_READ_BUFFER_LENGTH);
            } while (last_read != 0);
            

            throw std::runtime_error("Windows enviornment variable "
                + v
                + " has value that exceeds documented buffer lengh "
                + std::to_string(WIN_ENV_READ_BUFFER_LENGTH));
        }

        int child_status;
        const pid_t wpid = waitpid(pid, &child_status, 0);

        if (wpid != pid)
        {
            throw std::runtime_error("Wait for child process interrupted");
        }

        if (child_status != 0)
        {
            throw std::runtime_error("Execution of cmd.exe has failed");
        }

        if (total_read < WIN_LINE_END_LENGTH)
        {
            throw std::runtime_error("Execution of cmd.exe has provided too little data");
        }

        std::string ret(buffer.get(), total_read - WIN_LINE_END_LENGTH);
        return ret;
    }

    const std::string & _get_linux_launcher_dir()
    {
        const menphina::Platform p = menphina::get_current_platform();
        return (p == menphina::Platform::WSL) ? WSL_LAUNCHER_PATH : LINUX_LAUNCHER_PATH;
    }

    // TODO remove attribute
    [[maybe_unused]]
    std::string _to_windows_path(const std::string& wslPath)
    {
        if (wslPath.size() < WSL_PATH_START_MIN_LENGTH) [[unlikely]]
        {
            throw std::runtime_error("WSL path '" + wslPath + "' too short");
        }

        if (wslPath.find_first_of(WSL_WIN_PATH_START, 0) != 0) [[unlikely]]
        {
            throw std::runtime_error("Invalid WSL path " + wslPath);
        }

        const char driveLetter = wslPath[WSL_PATH_START_MIN_LENGTH - 1];
        if (!std::isalpha(driveLetter) || (wslPath.size() != WSL_PATH_START_MIN_LENGTH && wslPath[WSL_PATH_START_MIN_LENGTH] != GENERIC_PATH_DELIM)) [[unlikely]]
        {
            throw std::runtime_error("Invalid WSL path " + wslPath);
        }

        // Now, we convert to Windows.
        std::string ret;

        // We need to fit the string.
        // /mnt/c
        // becomes:
        // C: BACKSLASH
        ret.reserve(wslPath.size() - WSL_WIN_PATH_START.size() + 1);
        ret += std::toupper(driveLetter);
        ret += ':';

        for (auto ptr = wslPath.cbegin() + WSL_PATH_START_MIN_LENGTH; ptr != wslPath.cend(); ++ptr)
        {
            ret += (*ptr != GENERIC_PATH_DELIM) ? *ptr : WINDOWS_PATH_DELIM;
        }
    }

    // TODO remove attribute
    [[maybe_unused]]
    std::string _to_wsl_path(const std::string& winPath)
    {
        // We have to determine the drive letter, and swap the things.
        // while we coud just copy and replace, we will do this the hardway.

        // First, is this even worth doing.
        // C: is 2 characters, for example
        if (winPath.size() < 3) [[unlikely]]
        {
            throw std::runtime_error("Windows path '" + winPath + "' too short");
        }

        const char first = winPath.at(0);
        if (!std::isalpha(first) || winPath.at(1) != ':') [[unlikely]]
        {
            throw std::runtime_error("Invalid Windows path " + winPath);
        }

        std::string ret;

        // We need to fit the string.
        // C:
        // becomes:
        // /mnt + /x
        // And add 1 just to make sure we can fit a null since the docs unclear.
        ret.reserve(WSL_WIN_PATH_START.size() + winPath.size() + 1);

        ret.append(WSL_WIN_PATH_START);
        ret += GENERIC_PATH_DELIM;
        ret += std::tolower(first);
        
        for (auto ptr = winPath.cbegin() + 2; ptr != winPath.cend(); ++ptr)
        {
            ret += (*ptr != WINDOWS_PATH_DELIM) ? *ptr : GENERIC_PATH_DELIM;
        }

        return ret;
    }

#endif

    std::string _get_home()
    {
        // Documentation for getpwnam/getpwuid say to use HOME, so we use that.
#if defined ( __linux__ )
        const menphina::Platform p = menphina::get_current_platform();
        if (p == menphina::Platform::WSL)
        {
            return _get_win_env(WSL_HOME_VAR_NAME);
        }
#endif

        const char * home = std::getenv(HOME_VAR_NAME.c_str());

        if (home == nullptr)
        {
            throw std::runtime_error("Unable to read env " + HOME_VAR_NAME);
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

std::string menphina::path_basename(const std::string_view pathstr)
{
    const std::filesystem::path p(pathstr);
    return p.filename().string();
}

const std::string & menphina::get_user_home_directory()
{
    static const std::string h = _get_home();
    return h;
}

std::string menphina::path_join(const std::string_view a, const std::string_view b)
{
    std::filesystem::path ret(a);
    ret /= b;
    return ret;
}

bool menphina::path_exists(const std::string_view path)
{
    const std::filesystem::path fsp(path);
    return std::filesystem::exists(path);
}

const std::string & get_relative_launcher_config_dir()
{
#if defined ( __linux__ )
    static const std::string & launcherDir = _get_linux_launcher_dir();
    return launcherDir;
#else
    return LAUNCHER_PATH;
#endif
}
