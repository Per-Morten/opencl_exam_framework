#pragma once
#include <CL/cl.h>
#include <opencl_scope_guard.h>

namespace cp
{
    namespace opencl
    {
        class api
        {
        public: 
            api() = delete;
            api(cl_device_type device_type, const char* program_filepath, const char* arguments);
            ~api();

            cl_platform_id platform_id();
            cl_device_id device();
            cl_context context();
            cl_command_queue queue();
            cl_program program();
            
        private:
            void get_device_id(cl_device_type device_type);
            void create_context();
            void create_command_queue();
            void create_program(const char* program_filepath, const char* arguments);


            cl_platform_id m_platform_id{};
            cl_device_id m_device{};
            cl_context m_context{};
            cl_command_queue m_queue{};
            cl_program m_program{};
        };
    }
}