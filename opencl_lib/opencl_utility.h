#pragma once
#include <CL\cl.h>

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

    }
}