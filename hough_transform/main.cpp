#include <cp_lib.h>
#include <opencl_api.h>
#include <cp_image.h>
#include <opencl_utility.h>
#include <string>
#include <cmath>

template<class Create_Func,
         class... Args>
decltype(auto)
create_cl_resource(Create_Func func,
                   Args... args)
{
    cl_int error;
    auto item = func(args..., &error);
    if (error != CL_SUCCESS)
    {
        CP_ERROR("Could not create resource: %s",
                 cp::opencl::get_error(error));
    }

    return item;
}

template<class Opencl_Func,
         class... Args>
void
invoke_verify(Opencl_Func func,
              Args... args)
{
    cl_int error = func(args...);
    if (error != CL_SUCCESS)
    {
        CP_ERROR("Could not execute operation: %s",
                 cp::opencl::get_error(error));
    }
}


cp::image
create_mock_image(std::uint32_t height,
                  std::uint32_t width)
{
    cp::image image;
    image.height = height;
    image.width = width;
    image.pixels.resize(height * width * 3, 0);

    return image;
}

cp::opencl::buffer
create_buffer(cp::opencl::api& api,
              cl_mem_flags flags,
              cp::image& image)
{
    cp::opencl::buffer buffer =
        create_cl_resource(clCreateBuffer,
                           api.context(),
                           flags,
                           sizeof(std::uint8_t) * image.pixels.size(),
                           image.pixels.data());

    return buffer;
}



int
main(CP_UNUSED int argc,
     CP_UNUSED char** argv)
{
    cp_log_init();

    constexpr std::uint32_t step_amount = 180;
    std::string arguments = "-Ikernels/cp_lib/ -Werror -cl-std=CL1.2\
                             -DSTEP_AMOUNT=" + std::to_string(step_amount);
    cp::opencl::api api(CL_DEVICE_TYPE_CPU,
                        "kernels/hough_transform.cl",
                        arguments.c_str());

    auto h_input_image = cp::load_image("assets/hough_transform_image.png");
    auto d_input_image = create_buffer(api,
                                       CL_MEM_READ_ONLY |
                                       CL_MEM_COPY_HOST_PTR |
                                       CL_MEM_HOST_NO_ACCESS,
                                       h_input_image);

    std::vector<std::int32_t> h_distances(h_input_image.width * h_input_image.height * step_amount, -1);
    cp::opencl::buffer d_distances =
        create_cl_resource(clCreateBuffer,
                           api.context(),
                           CL_MEM_READ_WRITE |
                           CL_MEM_HOST_NO_ACCESS |
                           CL_MEM_COPY_HOST_PTR,
                           sizeof(std::int32_t) * h_distances.size(),
                           h_distances.data());

    std::vector<std::int32_t> h_angles(h_input_image.width * h_input_image.height * step_amount, -1);
    cp::opencl::buffer d_angles =
        create_cl_resource(clCreateBuffer,
                           api.context(),
                           CL_MEM_READ_WRITE |
                           CL_MEM_HOST_NO_ACCESS |
                           CL_MEM_COPY_HOST_PTR,
                           sizeof(std::int32_t) * h_angles.size(),
                           h_angles.data());

    auto h_output_image = create_mock_image(std::sqrt(std::pow(h_input_image.height,2) + std::pow(h_input_image.width,2)),
                                            step_amount);
    auto d_output_image = create_buffer(api,
                                        CL_MEM_WRITE_ONLY |
                                        CL_MEM_COPY_HOST_PTR |
                                        CL_MEM_HOST_READ_ONLY,
                                        h_output_image);

    cp::opencl::kernel hough_kernel =
        create_cl_resource(clCreateKernel,
                           api.program(),
                           "hough_transform");



    // Be skeptical here, if we get weird data,
    // It might be that the &d_input_image.data is weird.
    // Should not be relevant on any of the other cl functions
    // Unless they take a void* to data.
    // Alternative solutions:
    // d_input_image.data() rather than &d_input_image.data()
    invoke_verify(clSetKernelArg,
                  hough_kernel.data(),
                  0,
                  sizeof(cl_mem),
                  &d_input_image.data());

    invoke_verify(clSetKernelArg,
                  hough_kernel.data(),
                  1,
                  sizeof(std::uint32_t),
                  &h_input_image.width);

    invoke_verify(clSetKernelArg,
                  hough_kernel.data(),
                  2,
                  sizeof(cl_mem),
                  &d_distances.data());

    invoke_verify(clSetKernelArg,
                  hough_kernel.data(),
                  3,
                  sizeof(cl_mem),
                  &d_angles.data());

    cp::opencl::kernel hough_space =
        create_cl_resource(clCreateKernel,
                           api.program(),
                           "create_hough_space");

    invoke_verify(clSetKernelArg,
                  hough_space.data(),
                  0,
                  sizeof(cl_mem),
                  &d_output_image.data());

    invoke_verify(clSetKernelArg,
                  hough_space.data(),
                  1,
                  sizeof(std::uint32_t),
                  &step_amount);

    invoke_verify(clSetKernelArg,
                  hough_space.data(),
                  2,
                  sizeof(cl_mem),
                  &d_distances.data());

    invoke_verify(clSetKernelArg,
                  hough_space.data(),
                  3,
                  sizeof(cl_mem),
                  &d_angles.data());

    std::size_t work_size = h_input_image.width * h_input_image.height;
    invoke_verify(clEnqueueNDRangeKernel,
                  api.queue(), hough_kernel.data(),
                  1, nullptr,
                  &work_size,
                  nullptr, 0, nullptr, nullptr);

    invoke_verify(clFinish, api.queue());

    work_size = h_output_image.width * h_output_image.height;
    invoke_verify(clEnqueueNDRangeKernel,
                  api.queue(), hough_space.data(),
                  1, nullptr,
                  &work_size,
                  nullptr, 0, nullptr, nullptr);

    invoke_verify(clFinish, api.queue());

    invoke_verify(clEnqueueReadBuffer,
                  api.queue(),
                  d_output_image.data(),
                  CL_TRUE, 0,
                  sizeof(std::uint8_t) * h_output_image.pixels.size(),
                  h_output_image.pixels.data(),
                  0, nullptr, nullptr);

    cp::write_image("assets/output.png", h_output_image);


    cp_log_shutdown();

    return 0;
}
