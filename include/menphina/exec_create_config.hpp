/* Copyright 2024 isaki */

#ifndef __MENPHINA_CREATE_CONFIG_HPP__
#define __MENPHINA_CREATE_CONFIG_HPP__

#include "exec.hpp"

namespace menphina
{
    class ExecCreateConfig final : public Execution
    {
        public:
            virtual ~ExecCreateConfig();
            ExecCreateConfig();

            void run(const std::string& appConfigFile) override;

    };
}

#endif