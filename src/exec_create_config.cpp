/* Copyright 2024 isaki */

#include <string>
#include <fstream>

#include "menphina/json.hpp"
#include "menphina/platform.hpp"
#include "menphina/exec_create_config.hpp"

namespace
{
    // TODO
}

menphina::ExecCreateConfig::~ExecCreateConfig() {}

menphina::ExecCreateConfig::ExecCreateConfig() {}

void menphina::ExecCreateConfig::run(const std::string&)
{
    // We need to find the XIV Launcher directoy as well as
    // the penumbra and mare configurations. If we fail, we will
    // just mark them as empty string.

    // First things first, locate the XIVLauncher directory.
}
