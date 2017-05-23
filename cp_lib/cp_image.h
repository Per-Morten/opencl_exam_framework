#pragma once
#include <vector>
#include <cstdint>
#include <lodepng.h>


namespace cp
{
    ///////////////////////////////////////////////////////
    /// \brief  Simple struct for holding an image
    ///         indicated by the pixels, height and width.
    ///
    /// \var cp::image::pixels
    ///         vector of pixels within the image.
    ///         Stored in RGBA format.
    ///
    /// \var cp::image::height
    ///         Indicates the height of the image.
    ///
    /// \var cp::image::width
    ///         Indicates the width of the image.
    ///////////////////////////////////////////////////////
    struct image
    {
        image() = default;
        image(const std::vector<std::uint8_t>& pixels,
              std::uint32_t height,
              std::uint32_t width);

        std::vector<std::uint8_t> pixels{};
        std::uint32_t height{};
        std::uint32_t width{};
    };

    ///////////////////////////////////////////////////////
    /// \brief  Loads an image from the file indicated
    ///         by filpath.
    ///         Program terminates on error.
    ///
    /// \param  filepath
    ///         filepath to load the file from,
    ///         cannot be nullptr.
    ///
    /// \return image read from filepath
    ///////////////////////////////////////////////////////
    image
    load_image(const char* filepath,
               LodePNGColorType type = LCT_RGB);

    ///////////////////////////////////////////////////////
    /// \brief  Saves an to the file indicated by
    ///         filepath.
    ///         Program terminates on error.
    ///
    /// \param  filepath
    ///         Where to save the file,
    ///         cannot be nullptr.
    ///         Currently only .png files are supported.
    ///
    /// \param  image
    ///         The image to save.
    ///////////////////////////////////////////////////////
    void
    write_image(const char* filepath,
                const cp::image& image,
                LodePNGColorType type = LCT_RGB);
}
