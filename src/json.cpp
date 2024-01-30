/* Copyright 2024 isaki */

#include <string>
#include <ostream>
#include <stdexcept>
#include <utility>

#include "glaze/glaze.hpp"

#include "menphina/m_exception.hpp"
#include "menphina/json.hpp"


namespace
{
    // glz::opts {
    //   uint32_t format = json;
    //   bool comments = false; // Write out comments
    //   bool error_on_unknown_keys = true; // Error when an unknown key is encountered
    //   bool skip_null_members = true; // Skip writing out params in an object if the value is null
    //   bool use_hash_comparison = true; // Will replace some string equality checks with hash checks
    //   bool prettify = false; // Write out prettified JSON
    //   char indentation_char = ' '; // Prettified JSON indentation char
    //   uint8_t indentation_width = 3; // Prettified JSON indentation size
    //   bool shrink_to_fit = false; // Shrinks dynamic containers to new size to save memory
    //   bool write_type_info = true; // Write type info for meta objects in variants
    //   bool force_conformance = false; // Do not allow invalid json normally accepted such as comments, nan, inf.
    //   bool error_on_missing_keys = false; // Require all non nullable keys to be present in the object. Use
    //                                       // skip_null_members = false to require nullable members
    //   bool error_on_const_read = false; // Error if attempt is made to read into a const value, by 
    //                                     // default the value is skipped without error
    //   uint32_t layout = rowwise; // CSV row wise output/input
    //   bool quoted_num = false; // treat numbers as quoted or array-like types as having quoted numbers
    //   bool number = false; // read numbers as strings and write these string as numbers
    //   bool raw = false; // write out string like values without quotes
    //   bool raw_ttring = false; // do not decode/encode escaped characters for strings (improves read/write performance)
    // };

    inline constexpr glz::opts JSON_WRITE_SETTINGS = {
        .skip_null_members = false,
        .prettify = true,
        .indentation_char = ' ',
        .indentation_width = 2,
        .force_conformance = true
    };

    inline constexpr glz::opts PLUGIN_STRUCT_READ_SETTINGS = {
        .error_on_unknown_keys = false,
        .error_on_missing_keys = true,
        .error_on_const_read = true
    };

    inline constexpr glz::opts MENPHINA_STRUCT_READ_SETTINGS = {
        .error_on_unknown_keys = true,
        .error_on_missing_keys = true,
        .error_on_const_read = true
    };

    // Exception adapters, inspired by Glaze's own exception types (which dont have a write file version)
    template<auto O = glz::opts{}, class T>
    void _read_json_file(T& value, const std::string& file)
    {
        std::string buffer {};
        const auto ec = glz::read_file_json<O, T>(value, file, buffer);
    
        if (ec == glz::error_code::file_open_failure)
        {
            throw menphina::file_open_exception(file, true);
        }
        else if (ec)
        {
            throw std::runtime_error("json read error: " + glz::format_error(ec, buffer));
        }
    }

    template<auto O = glz::opts{}, class T>
    void _write_json_file(T&& value, const std::string& file)
    {
        const auto ec = glz::write_file_json<O, T>(std::forward<T>(value), file, std::string {});
    
        if (ec == glz::error_code::file_open_failure)
        {
            throw menphina::file_open_exception(file, false);
        }
        else if (ec)
        {
            throw std::runtime_error("json write error");
        }
    }
}

void menphina::read_json_file(penumbra_config_t& obj, const std::string& jsonFile)
{
    _read_json_file<PLUGIN_STRUCT_READ_SETTINGS, penumbra_config_t>(obj, jsonFile);
}

void menphina::read_json_file(mare_config_t& obj, const std::string& jsonFile)
{
    _read_json_file<PLUGIN_STRUCT_READ_SETTINGS, mare_config_t>(obj, jsonFile);
}

void menphina::read_json_file(app_config_t& obj, const std::string& jsonFile)
{
    _read_json_file<MENPHINA_STRUCT_READ_SETTINGS, app_config_t>(obj, jsonFile);
}

void menphina::write_json_file(const app_config_t& obj, const std::string& jsonFile)
{
    _write_json_file<JSON_WRITE_SETTINGS, const app_config_t&>(obj, jsonFile);
}
