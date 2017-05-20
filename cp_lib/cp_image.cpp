#include <cp_image.h>
#include <cp_lib.h>
#include <lodepng.h>

cp::image::image(const std::vector<std::uint8_t>& pixels_,
                 std::uint32_t height_,
                 std::uint32_t width_)
    : pixels(pixels_)
    , height(height_)
    , width(width_)
{
}

cp::image
cp::load_image(const char* filepath)
{
    std::vector<std::uint8_t> pixels;
    unsigned width;
    unsigned height;

    const auto err_c = lodepng::decode(pixels,
                                       width,
                                       height,
                                       filepath);

    if (err_c)
    {
        CP_ERROR("Could not decode image: %s",
                 lodepng_error_text(err_c));
    }

    return
    {
        pixels,
        static_cast<std::uint32_t>(height),
        static_cast<std::uint32_t>(width)
    };
}

void
cp::write_image(const char* filepath,
                const cp::image& image)
{
    const auto err_c =
        lodepng::encode(filepath,
                        image.pixels,
                        static_cast<unsigned>(image.width),
                        static_cast<unsigned>(image.height));

    if (err_c)
    {
        CP_ERROR("Could not encode image: %s",
                 lodepng_error_text(err_c));
    }
}
