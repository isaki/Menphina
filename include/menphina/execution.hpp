#ifndef __MENPHINA_EXECUTION_HPP__
#define __MENPHINA_EXECUTION_HPP__

#include <string>
#include <menphina/appconfig.hpp>

namespace menphina
{
    class Execution
    {
        public:
            virtual ~Execution() {}

            virtual int run(const AppConfig& config) noexcept = 0;
        protected:
            Execution() {}
    };
}

#endif