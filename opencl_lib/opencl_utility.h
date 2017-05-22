#pragma once
#include <CL/cl.h>
#include <cp_lib.h>

namespace cp
{
    namespace opencl
    {
        ///////////////////////////////////////////////////////////////////////////////
        /// Taken from:
        /// https://github.com/HandsOnOpenCL/Exercises-Solutions/blob/master/Exercises/C_common/err_code.h
        ///////////////////////////////////////////////////////////////////////////////
        const char* get_error(const cl_int status);
        long int file_size(const char* path);

        char* read_program(const char* path);

        ///////////////////////////////////////////////////
        /// \brief Creates an OpenCL resource, and does
        ///        error checking.
        ///        Crashes on failure.
        ///////////////////////////////////////////////////
        template<class Create_Func, class... Args>
        decltype(auto)
        create_resource(Create_Func func, Args... args);

        ///////////////////////////////////////////////////
        /// \brief Invokes an OpenCL function and does
        ///        error checking.
        ///        Crashes on failure.
        ///////////////////////////////////////////////////
        template<class Opencl_Func, class... Args>
        void
        invoke(Opencl_Func func, Args... args);
    }
}

#include <opencl_utility.tpp>
