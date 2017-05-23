#include <cp_lib_types.h>

__kernel void greyscale(__read_only image2d_t in_image,
                        __write_only image2d_t out_image)
{
    int2 pos = {get_global_id(0), get_global_id(1)};

    uint4 colors = read_imageui(in_image, pos);

    //uint8_t avg = (colors.x + colors.y + colors.z) / 3;
    uint8_t avg = (colors.x * 0.21f + colors.y * 0.72f + colors.z * 0.07f);
    colors.x = avg;
    colors.y = avg;
    colors.z = avg;

    write_imageui(out_image, pos, colors);
}

