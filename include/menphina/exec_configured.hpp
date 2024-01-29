/* Copyright 2024 isaki */

#ifndef __MENPHINA_CONFIGURED_EXEC_HPP__
#define __MENPHINA_CONFIGURED_EXEC_HPP__

#include "exec.hpp"
#include "json.hpp"

namespace menphina
{
    class ExecConfigured : public Execution
    {
        public:
            virtual ~ExecConfigured();
            void run(const std::string& appConfigFile) override final;
        protected:
            ExecConfigured();

            virtual void run(const struct app_config_s ac) = 0;
    };
}

#endif