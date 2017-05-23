#include <cp_lib.h>
#include <opencl_api.h>
#include <cp_image.h>
#include <opencl_utility.h>
#include <string>
#include <cmath>



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
        cp::opencl::create_resource(clCreateBuffer,
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
    namespace cpcl = cp::opencl;
    cp_log_init();

    constexpr std::uint32_t step_amount = 180;
    std::string arguments = "-Ikernels/cp_lib/ -Werror -cl-std=CL1.2\
                             -DSTEP_AMOUNT=" + std::to_string(step_amount);
    cpcl::api api(CL_DEVICE_TYPE_CPU,
                        "kernels/hough_transform.cl",
                        arguments.c_str());

    auto h_input_image = cp::load_image("assets/hough_transform_image.png");
    auto d_input_image = create_buffer(api,
                                       CL_MEM_READ_ONLY |
                                       CL_MEM_COPY_HOST_PTR |
                                       CL_MEM_HOST_NO_ACCESS,
                                       h_input_image);

    std::vector<std::int32_t> h_distances(h_input_image.width * h_input_image.height * step_amount, -1);
    cpcl::buffer d_distances =
        cpcl::create_resource(clCreateBuffer,
                              api.context(),
                              CL_MEM_READ_WRITE |
                              CL_MEM_HOST_NO_ACCESS |
                              CL_MEM_COPY_HOST_PTR,
                              sizeof(std::int32_t) * h_distances.size(),
                              h_distances.data());

    std::vector<std::int32_t> h_angles(h_input_image.width * h_input_image.height * step_amount, -1);
    cpcl::buffer d_angles =
        cpcl::create_resource(clCreateBuffer,
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

    cpcl::kernel hough_kernel =
        cpcl::create_resource(clCreateKernel,
                              api.program(),
                              "hough_transform");

    cpcl::invoke(clSetKernelArg,
                 hough_kernel.data(),
                 0,
                 sizeof(cl_mem),
                 &d_input_image.data());

    cpcl::invoke(clSetKernelArg,
                 hough_kernel.data(),
                 1,
                 sizeof(std::uint32_t),
                 &h_input_image.width);

    cpcl::invoke(clSetKernelArg,
                 hough_kernel.data(),
                 2,
                 sizeof(cl_mem),
                 &d_distances.data());

    cpcl::invoke(clSetKernelArg,
                 hough_kernel.data(),
                 3,
                 sizeof(cl_mem),
                 &d_angles.data());

    cpcl::kernel hough_space =
        cpcl::create_resource(clCreateKernel,
                              api.program(),
                              "create_hough_space");

    cpcl::invoke(clSetKernelArg,
                 hough_space.data(),
                 0,
                 sizeof(cl_mem),
                 &d_output_image.data());

    cpcl::invoke(clSetKernelArg,
                 hough_space.data(),
                 1,
                 sizeof(std::uint32_t),
                 &step_amount);

    cpcl::invoke(clSetKernelArg,
                 hough_space.data(),
                 2,
                 sizeof(cl_mem),
                 &d_distances.data());

    cpcl::invoke(clSetKernelArg,
                 hough_space.data(),
                 3,
                 sizeof(cl_mem),
                 &d_angles.data());

    std::size_t work_size = h_input_image.width * h_input_image.height;
    cpcl::invoke(clEnqueueNDRangeKernel,
                 api.queue(), hough_kernel.data(),
                 1, nullptr,
                 &work_size,
                 nullptr, 0, nullptr, nullptr);

    cpcl::invoke(clFinish, api.queue());

    work_size = h_output_image.width * h_output_image.height;
    cpcl::invoke(clEnqueueNDRangeKernel,
                 api.queue(), hough_space.data(),
                 1, nullptr,
                 &work_size,
                 nullptr, 0, nullptr, nullptr);

    cpcl::invoke(clFinish, api.queue());

    cpcl::invoke(clEnqueueReadBuffer,
                 api.queue(),
                 d_output_image.data(),
                 CL_TRUE, 0,
                 sizeof(std::uint8_t) * h_output_image.pixels.size(),
                 h_output_image.pixels.data(),
                 0, nullptr, nullptr);

    cp::write_image("assets/hough_output.png", h_output_image);


    cp_log_shutdown();

    return 0;
}
