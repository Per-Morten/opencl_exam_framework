#pragma once
#include <string>
#include <CL/cl.h>
#include <algorithm>
#include <vector>
#include <cstring>

namespace cp
{
    struct cmd_params
    {
        cl_device_type device_type;
        std::string in_filepath;
        std::string out_filepath;
    };

    inline
    cmd_params
    parse_cmd(int argc,
              char** argv,
              const char* default_in,
              const char* default_out)
    {
        cmd_params params;
        if (argc < 2)
        {
            params.device_type = CL_DEVICE_TYPE_GPU;
            params.in_filepath = default_in;
            params.out_filepath = default_out;
            return params;
        }

        std::vector<char*> strings(argc - 1);
        for (int i = 1; i < argc; ++i)
        {
            strings[i - 1] = argv[i];
        }

        auto type_itr = std::find_if(std::cbegin(strings),
                                     std::cend(strings),
                                     [](const char* item)
                                     { return strcmp(item, "-device"); });

        if (type_itr != std::cend(strings))
        {
            auto type_string = *std::next(type_itr);
            params.device_type = (strcmp(type_string, "cpu") == 0)
            ? CL_DEVICE_TYPE_CPU
            : CL_DEVICE_TYPE_GPU;
        }

        auto in_itr = std::find_if(std::cbegin(strings),
                                   std::cend(strings),
                                   [](const auto& item)
                                   { return strcmp(item, "-in") == 0; });

        params.in_filepath = (in_itr != std::cend(strings))
                           ? *std::next(in_itr)
                           : default_in;

        auto out_itr = std::find_if(std::cbegin(strings),
                                    std::cend(strings),
                                    [](const auto& item)
                                    { return strcmp(item, "-out") == 0; });

        params.out_filepath = (out_itr != std::cend(strings))
                            ? *std::next(out_itr)
                            : default_out;

        return params;
    }
}
