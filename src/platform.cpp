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
#endif

#include "menphina/m_exception.hpp"
#include "menphina/platform.hpp"

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

    // ... 32,767... for an environment variable...
    inline constexpr size_t WIN_ENV_LENGTH = 32767;
    
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
        auto buffer = std::unique_ptr<char[]>(new char[WIN_ENV_LENGTH]());
        char * ptr = buffer.get();
        size_t total_read = 0;
        size_t last_read = 0;

        do
        {
            last_read = read(fd, ptr, WIN_ENV_LENGTH - total_read);

            if (last_read == 0)
            {
                break;
            }

            total_read += last_read;
            ptr += last_read;
        } while (total_read < WIN_ENV_LENGTH);

        // This shouldn't happen unless our assumptions break, which, with WSL, you never know.
        if (last_read != 0)
        {
            // Drain down the buffer
            do
            {
                last_read = read(fd, buffer.get(), WIN_ENV_LENGTH);
            } while (last_read != 0);
            

            throw std::runtime_error("Windows enviornment variable "
                + v
                + " has value that exceeds documented buffer lengh "
                + std::to_string(WIN_ENV_LENGTH));
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

        std::string ret(buffer.get(), total_read);
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
