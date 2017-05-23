#include <utility>

template<class Create_Func, class... Args>
decltype(auto)
cp::opencl::create_resource(Create_Func func,
                            Args&&... args)
{
    cl_int error;
    auto item = func(std::forward<Args>(args)..., &error);
    if (error != CL_SUCCESS)
    {
        CP_ERROR("Could not create resource: %s",
                 cp::opencl::get_error(error));
    }

    return item;
}

template<class Opencl_Func, class... Args>
void
cp::opencl::invoke(Opencl_Func func,
                   Args&&... args)
{
    cl_int error = func(std::forward<Args>(args)...);
    if (error != CL_SUCCESS)
    {
        CP_ERROR("Could not execute operation: %s",
                 cp::opencl::get_error(error));
    }
}
