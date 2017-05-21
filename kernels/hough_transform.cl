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
        const float rad_angle = (float)i * STEP_SIZE;
        //const float rad_angle = radians((float)i);

        const float c = -x_pos * cos(rad_angle) - y_pos * sin(rad_angle);

        const float dist = fabs(0.0f + 0.0f + c) /
                                sqrt(pow(cos(rad_angle), 2.0f) +
                                     pow(sin(rad_angle), 2.0f));

        angles[i] = (int32_t)degrees(rad_angle);
        distances[i] = (int32_t)dist;


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
    size_t image_pos = get_global_id(0) * 3;

    size_t id = get_global_id(0);
    size_t x_pos = id % width;
    size_t y_pos = id / width;

    size_t pixel_count = get_global_size(0);

    uint8_t match = 0;
    for (size_t i = 0; i < pixel_count; i += STEP_AMOUNT)
    {
        for (size_t j = 0; j < STEP_AMOUNT; ++j)
        {
            if (distances[i + j] == y_pos &&
                angles[i + j] == x_pos)
            {
                if (match != 255)
                {
                    ++match;
                }
            }
        }
    }


    if (match > 0)
    {
        //printf("%zu\n", (size_t)match);
        image[image_pos + 0] = match + 100;
        image[image_pos + 1] = match + 100;
        image[image_pos + 2] = match + 100;
    }

}


// kernel 1:
//  if datapoint:
//      for each angle
//          plot distance from line to origin
//              save dist and angle
//
// kernel 2:
//  if datapoint
//
