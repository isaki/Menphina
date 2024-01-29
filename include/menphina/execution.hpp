/* Copyright 2024 isaki */

#ifndef __MENPHINA_EXECUTION_HPP__
#define __MENPHINA_EXECUTION_HPP__

#include <string>

namespace menphina
{
    class Execution
    {
        public:
            virtual ~Execution() {}

            virtual int run(const std::string& appConfigFile) noexcept = 0;
        protected:
            Execution() {}
    };
}

#endif