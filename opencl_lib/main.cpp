#include <opencl_api.h>
#include <cp_lib.h>
int main()
{
    cp_log_init();
    cp::opencl::api api(CL_DEVICE_TYPE_GPU, "kernels/hough_transform.cl", "-Ikernels/cp_lib/ -Werror -cl-std=CL1.2");
    return 0;
}