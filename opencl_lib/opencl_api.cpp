#include <opencl_api.h>
#include <cp_lib.h>
#include <opencl_utility.h>

namespace cp
{
    namespace opencl
    {

        api::api(cl_device_type device_type,
                 const char* program_filepath,
                 const char* arguments)
        {
            get_device_id(device_type);
            create_context();
            create_command_queue();
            create_program(program_filepath, arguments);
        }

        api::~api()
        {
            clReleaseProgram(program);
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
        }

        void api::get_device_id(cl_device_type device_type)
        {
            cl_uint id_count;
            cl_platform_id platform_ids[1 << 8];
            cl_int status = clGetPlatformIDs(1 << 8, platform_ids, &id_count);
            if (status != CL_SUCCESS || id_count == 0)
            {
                CP_ERROR("Failed to get platform ids, error: %s, id_count: %u", get_error(status), id_count);
            }

            for (cl_uint i = 0; i < id_count; ++i)
            {
                status = clGetDeviceIDs(platform_ids[i], device_type, 1, &device, nullptr);
                platform_id = platform_ids[i];
                if (status == CL_SUCCESS)
                {
                    break;
                }
            }

            if (device == nullptr)
            {
                CP_ERROR("Could not set device id, error: %s", get_error(status));
            }
        }

        void api::create_context()
        {
            cl_int status;
            context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &status);

            if (status != CL_SUCCESS)
            {
                CP_ERROR("Count not create context, error: %s", get_error(status));
            }
        }

        void api::create_command_queue()
        {

            cl_int error;
#ifdef _MSC_VER
            queue = clCreateCommandQueue(context, device, 0, &error);
#else
            queue = clCreateCommandQueueWithProperties(context, device, nullptr, &error);
#endif
            if (error != CL_SUCCESS)
            {
                clReleaseContext(context);
                CP_ERROR("Could not create command queue, error: %s", get_error(error));
            }

        }

        void api::create_program(const char* program_filepath,
                                 const char* arguments)
        {
            char* source = read_program(program_filepath);

            if (source == nullptr)
            {
                clReleaseCommandQueue(queue);
                clReleaseContext(context);
                CP_ERROR("Failed to read program");
            }

            cl_int build_error;
            program = clCreateProgramWithSource(context, 1,
                                                const_cast<const char**>(&source),
                                                nullptr,
                                                &build_error);

            free(source);

            if (build_error != CL_SUCCESS)
            {
                clReleaseCommandQueue(queue);
                clReleaseContext(context);
                CP_WARN("Could not create program, error: %s", get_error(build_error));

            }

            build_error = clBuildProgram(program, 1,
                                         &device, arguments,
                                         nullptr, nullptr);

            if (build_error != CL_SUCCESS)
            {
                size_t length = 0;
                char buffer[2048];

                cl_int error = clGetProgramBuildInfo(program,
                                                     device,
                                                     CL_PROGRAM_BUILD_LOG,
                                                     sizeof(buffer),
                                                     buffer,
                                                     &length);

                if (error != CL_SUCCESS)
                {
                    CP_WARN("Could not create program, error: %s", get_error(error));
                }

                clReleaseCommandQueue(queue);
                clReleaseContext(context);
                clReleaseProgram(program);
                CP_ERROR("Could not build program, error: %s\nBuild info: %s", 
                         get_error(build_error), 
                         buffer);

            }
        }

    }
}