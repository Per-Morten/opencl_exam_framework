#pragma once
#include <CL/cl.h>

namespace cp
{
    namespace opencl
    {
        class api
        {
        private:
            void get_device_id(cl_device_type device_type);
            void create_context();
            void create_command_queue();
            void create_program(const char* program_filepath, const char* arguments);


        public:
            cl_platform_id platform_id{};
            cl_device_id device{};
            cl_context context{};
            cl_command_queue queue{};
            cl_program program{};

            api(cl_device_type device_type, const char* program_filepath, const char* arguments);
            api() = delete;
            ~api();
        };
    }
}