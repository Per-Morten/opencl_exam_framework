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
            clReleaseProgram(m_program);
            clReleaseCommandQueue(m_queue);
            clReleaseContext(m_context);
        }

        cl_platform_id api::platform_id()
        {
            return m_platform_id;
        }

        cl_device_id api::device()
        {
            return m_device;
        }

        cl_context api::context()
        {
            return m_context;
        }

        cl_command_queue api::queue()
        {
            return m_queue;
        }

        cl_program api::program()
        {
            return m_program;
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
                status = clGetDeviceIDs(platform_ids[i], device_type, 1, &m_device, nullptr);
                m_platform_id = platform_ids[i];
                if (status == CL_SUCCESS)
                {
                    break;
                }
            }

            if (m_device == nullptr)
            {
                CP_ERROR("Could not set device id, error: %s", get_error(status));
            }
        }

        void api::create_context()
        {
            cl_int status;
            m_context = clCreateContext(nullptr, 1, &m_device, nullptr, nullptr, &status);

            if (status != CL_SUCCESS)
            {
                CP_ERROR("Count not create context, error: %s", get_error(status));
            }
        }

        void api::create_command_queue()
        {
            char buffer[1024];
            cl_int error = clGetPlatformInfo(m_platform_id,
                                             CL_PLATFORM_VERSION,
                                             sizeof(buffer),
                                             buffer,
                                             nullptr);

            if (error != CL_SUCCESS)
            {
                clReleaseContext(m_context);
                CP_ERROR("Could not get platform info, error: %s",
                         get_error(error));
            }

            int32_t version[2] = {0};
            sscanf(buffer,"OpenCL %d.%d", &version[0], &version[1]);

        CP_PUSH_WARNING_DEPRECATED
        #ifdef _MSC_VER
            m_queue = clCreateCommandQueue(m_context, m_device, 0, &error);
        #else
            m_queue = (version[0] == 2)
                       ? clCreateCommandQueueWithProperties(m_context, m_device, NULL, &error)
                       : clCreateCommandQueue(m_context, m_device, 0, &error);
        #endif
        CP_POP_WARNING

            if (error != CL_SUCCESS)
            {
                clReleaseContext(m_context);
                CP_ERROR("Could not create command queue, error: %s",
                         get_error(error));
            }
        }

        void api::create_program(const char* program_filepath,
                                 const char* arguments)
        {
            char* source = read_program(program_filepath);

            if (source == nullptr)
            {
                clReleaseCommandQueue(m_queue);
                clReleaseContext(m_context);
                CP_ERROR("Failed to read program");
            }

            cl_int build_error;
            m_program = clCreateProgramWithSource(m_context, 1,
                                                  const_cast<const char**>(&source),
                                                  nullptr,
                                                  &build_error);

            free(source);

            if (build_error != CL_SUCCESS)
            {
                clReleaseCommandQueue(m_queue);
                clReleaseContext(m_context);
                CP_WARN("Could not create program, error: %s", get_error(build_error));

            }

            build_error = clBuildProgram(m_program, 1,
                                         &m_device, arguments,
                                         nullptr, nullptr);

            if (build_error != CL_SUCCESS)
            {
                size_t length = 0;
                char buffer[2048];

                cl_int error = clGetProgramBuildInfo(m_program,
                                                     m_device,
                                                     CL_PROGRAM_BUILD_LOG,
                                                     sizeof(buffer),
                                                     buffer,
                                                     &length);

                if (error != CL_SUCCESS)
                {
                    CP_WARN("Could not create program, error: %s", get_error(error));
                }

                clReleaseCommandQueue(m_queue);
                clReleaseContext(m_context);
                clReleaseProgram(m_program);
                CP_ERROR("Could not build program, error: %s\nBuild info: %s",
                         get_error(build_error),
                         buffer);

            }
        }
    }
}
