/* Copyright 2024 isaki */

#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>

#include "boost/program_options.hpp"
#include "menphina_internal/config.hpp"

#include "menphina/platform.hpp"
#include "menphina/executions.hpp"

namespace po = boost::program_options;

namespace
{
    const std::string MODE_CLEAN { "clean" };
    const std::string MODE_PACKAGE { "package" };
    const std::string MODE_DEPLOY { "deploy" };
    const std::string MODE_CREATE_CONF { "create-config"};

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
    menphina::Execution * exec;
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

            std::cout << std::endl
                << "  create-config" << std::endl
                << "    attempts to locate the required files and directories to automatically" << std::endl
                << "    create the " << CONFIG_NAME << " file" << std::endl;

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
                exec = nullptr;
            }
            else if (mode == MODE_PACKAGE)
            {
                exec = nullptr;
            }
            else if (mode == MODE_DEPLOY)
            {
                exec = nullptr;
            }
            else if (mode == MODE_CREATE_CONF)
            {
                exec = new menphina::ExecCreateConfig();
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

    // TODO: The null check isnt needed once all execs are built.
    if (exec)
    {
        const std::string appConfigFile = _get_config_file();
        exec->run(appConfigFile);
        delete exec;
    }

    return 0;
}
