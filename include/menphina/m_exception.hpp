/* Copyright 2024 isaki */

#ifndef __MENPHINA_EX_HPP__
#define __MENPHINA_EX_HPP__

#include <exception>
#include <string>
#include <string_view>

namespace menphina
{
    class errno_exception final : public std::exception
    {
        public:
            errno_exception(const std::string_view& msg, const int err);
            ~errno_exception();

            const char* what() const throw() override;

        private:
            std::string m_msg;
    };

    class str_exception final : public std::exception
    {
        public:
            str_exception(const std::string_view& msg);
            ~str_exception();

            const char* what() const throw() override;

        private:
            const std::string m_msg;
    };
}

#endif