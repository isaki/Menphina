#include <string>
#include <string_view>
#include <cstring>
#include "menphina/m_exception.hpp"

/* errno_exception */

menphina::errno_exception::~errno_exception() {}

menphina::errno_exception::errno_exception(const std::string_view& msg, const int err) : m_msg(msg)
{
    const char * errstr = strerror(err);
    m_msg.append(" (")
        .append(errstr)
        .append(" [")
        .append(std::to_string(err))
        .append("])");
}

const char * menphina::errno_exception::what() const throw()
{
    return m_msg.c_str();
}

/* str_exception */
menphina::str_exception::~str_exception() {}

menphina::str_exception::str_exception(const std::string_view& msg) : m_msg(msg) {}

const char * menphina::str_exception::what() const throw()
{
    return m_msg.c_str();
}