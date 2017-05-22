#ifndef STEP_AMOUNT
#error "Need to specity step amount"
#endif

#define STEP_SIZE M_PI_F / STEP_AMOUNT

#include <cp_lib_types.h>

__kernel void hough_transform(__global const uint8_t* image,
                              uint32_t width,
                              __global int32_t* out_distances,
                              __global int32_t* out_angles)
{
    size_t image_pos = get_global_id(0) * 3;

    if (image[image_pos + 0] > 0 &&
        image[image_pos + 1] > 0 &&
        image[image_pos + 2] > 0)
    {
        return;
    }

    const size_t id = get_global_id(0);
    float x_pos = (float)(id % width);
    float y_pos = (float)(id / width);

    int32_t distances[STEP_AMOUNT];
    int32_t angles[STEP_AMOUNT];

    for (size_t i = 0; i < STEP_AMOUNT; i++)
    {
        const float rad_angle = i * STEP_SIZE;

        const float c = -x_pos * cos(rad_angle) - y_pos * sin(rad_angle);

        const float dist = fabs(c) /
                                sqrt(pow(cos(rad_angle), 2.0f) +
                                     pow(sin(rad_angle), 2.0f));

        angles[i] = round(degrees(rad_angle));
        distances[i] = round(dist);


        //printf("x: %5.2f, y: %5.2f, angle: %5.2f, dist: %5.2f\n",
        //       x_pos, y_pos, (float)angles[i], dist);
    }

    for (size_t i = 0; i < STEP_AMOUNT; ++i)
    {
        out_angles[i + id] = angles[i];
        out_distances[i + id] = distances[i];
    }
}

__kernel void create_hough_space(__global uint8_t* image,
                                 uint32_t width,
                                 __global const int32_t* distances,
                                 __global const int32_t* angles)
{
    size_t id = get_global_id(0);
    int32_t x_pos = id % width;
    int32_t y_pos = id / width;

    size_t pixel_count = get_global_size(0);

    uint8_t match = 0;
    for (size_t i = 0; i < pixel_count; i += STEP_AMOUNT)
    {
        for (size_t j = 0; j < STEP_AMOUNT; ++j)
        {
            if (distances[i + j] == y_pos &&
                angles[i + j] == x_pos)
            {
                if (match < 255)
                {
                    ++match;
                }
            }
        }
    }

    size_t image_pos = get_global_id(0) * 3;

    if (match > 0)
    {
        //printf("%zu\n", (size_t)match);
        image[image_pos + 0] = match * 10;
        image[image_pos + 1] = match * 10;
        image[image_pos + 2] = match * 10;
    }

    if (match > 100)
    {
        printf("x: %d, y: %d\n", x_pos, y_pos);
    }

}
