#pragma once
#include <CL/cl.h>

namespace cp
{
    namespace opencl
    {

        template<class T,
                 class Destructor>
        class scope_guard
        {
        public:
            scope_guard() = default;

            scope_guard(T value)
                : m_data(value)
            {

            }

            scope_guard(const scope_guard&) = delete;
            scope_guard& operator=(const scope_guard&) = delete;

            scope_guard(const scope_guard&& rhs)
                : m_data(rhs.data)
            {
                rhs.data = nullptr;
            }

            scope_guard& operator=(const scope_guard&& rhs)
            {
                if (this != &rhs)
                {
                    Destructor()(m_data);
                    m_data = rhs.data;
                    rhs.data = nullptr;
                }

                return *this;
            }

            ~scope_guard()
            {
                Destructor()(m_data);
            }

            T data()
            {
                return m_data;
            }

        private:
            T m_data{};

        };

        // We were not allowed to use lambdas,
        namespace opencl_wrapper_impl
        {
            struct kernel_destructor
            {
                void operator()(cl_kernel kernel)
                {
                    if (kernel)
                    {
                        clReleaseKernel(kernel);
                    }
                }
            };

            struct mem_obj_destructor
            {
                void operator()(cl_mem mem_obj)
                {
                    if (mem_obj)
                    {
                        clReleaseMemObject(mem_obj);
                    }
                }
            };
        }

        using buffer = scope_guard<cl_mem, opencl_wrapper_impl::mem_obj_destructor>;
        using kernel = scope_guard<cl_kernel, opencl_wrapper_impl::kernel_destructor>;
    }
}
