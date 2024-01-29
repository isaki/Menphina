/* Copyright 2024 isaki */

#include <string>
#include "menphina/json.hpp"
#include "menphina/exec_configured.hpp"

menphina::ExecConfigured::~ExecConfigured() {}

menphina::ExecConfigured::ExecConfigured() {}

void menphina::ExecConfigured::run(const std::string& appConfigFile)
{
    struct menphina::app_config_s ac;
    menphina::read_json_file(ac, appConfigFile);
    run(ac);
}