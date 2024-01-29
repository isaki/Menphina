/* Copyright 2024 isaki */

#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>

#include "boost/program_options.hpp"
#include "menphina_internal/config.hpp"
#include "menphina/appconfig.hpp"
#include "menphina/util.hpp"
#include "menphina/execution.hpp"

namespace po = boost::program_options;

namespace
{
    const std::string MODE_CLEAN { "clean" };
    const std::string MODE_PACKAGE { "package" };
    const std::string MODE_DEPLOY { "deploy" };

    const std::string CONFIG_NAME { ".menphina.json" };

    std::string _argv_basename(const char * name)
    {
        const std::string_view tmp(name);
        return menphina::path_basename(tmp);
    }

    std::string _get_config_file()
    {
        const std::string home = menphina::get_user_home_directory();
        return menphina::path_join(home, CONFIG_NAME);
    }
}

int main(int argc, char ** argv)
{
    //menphina::Execution * exec;
    try
    {    
        // Declare the supported options.
        po::options_description desc("Generic options");
        desc.add_options()
            ("help", "print this message message")
            ("version", "display version information")
        ;

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("mode", po::value<std::string>(), "The operating mode: clean, deploy, or package")
        ;

        po::options_description all;
        all.add(desc).add(hidden);

        po::positional_options_description posdesc;
        posdesc.add("mode", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(all).positional(posdesc).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            const std::string name = _argv_basename(argv[0]);
            std::cout << name << " <mode> " << std::endl << std::endl;
            std::cout << desc << std::endl;

            std::cout << "Modes:" << std::endl;

            std::cout << std::endl
                << "  clean" << std::endl
                << "    analyzes and cleans orphaned data from the Penumbra configuration" << std::endl
                << "    directory" << std::endl;

            std::cout << std::endl
                << "  package" << std::endl
                << "    packages all configured mod data into a deployment package (xmpkg)" << std::endl;

            std::cout << std::endl
                << "  deploy" << std::endl
                << "    deploys all configured (if present) mod data from a deployment package" << std::endl;

            std::cout << std::endl;

            return 1;
        }

        if (vm.count("version"))
        {
            const std::string name = _argv_basename(argv[0]);
            std::cout << name
                << " v"
                << menphina::cmake::project_version
                << std::endl;

            return 0;
        }

        if (vm.count("mode"))
        {
            const std::string mode = vm["mode"].as<std::string>();

            if (mode == MODE_CLEAN)
            {
                // We would allocate clean object
            }
            else if (mode == MODE_PACKAGE)
            {
                // We would allocate package object
            }
            else if (mode == MODE_DEPLOY)
            {
                // We would allocate deployment object
            }
            else
            {
                // Input error
                std::cerr << "Invalid operating mode: " << mode << std::endl;
                return 1;
            }
        }
        else
        {
            std::cerr << "No operating mode specified; please run with --help" << std::endl;
            return 1;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // const int ret = exec->run();
    // delete exec;
    // return ret;

    struct menphina::app_config_s appConfig;

    try
    {
        const std::string configFile = _get_config_file();
        std::cout << "Loading configuration from " << configFile << std::endl;

        menphina::read_json_file<struct menphina::app_config_s>(appConfig, configFile);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to load configuration: " << e.what() << std::endl;
        return 2;
    }

    std::cout << "Menphina configuration:" << appConfig;

    return 0;
}
