#include <string>
#include <string_view>
#include <cstring>
#include "menphina/m_exception.hpp"

/* errno_exception */

menphina::errno_exception::~errno_exception() {}

menphina::errno_exception::errno_exception(const std::string_view msg, const int err) : m_msg(msg)
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

/* file_open_excetion */
menphina::file_open_exception::~file_open_exception() {}

menphina::file_open_exception::file_open_exception(const std::string_view file, const bool isRead)
{
    m_msg.append("failed to open file for ")
        .append((isRead) ? "read" : "write")
        .append(": ")
        .append(file);
}

const char * menphina::file_open_exception::what() const throw()
{
    return m_msg.c_str();
}
