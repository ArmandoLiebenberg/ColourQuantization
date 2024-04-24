#include <cstdio>
#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "../include/stb_image.h"
    #include "../include/stb_image_write.h"
}

// Loads as RGBA... even if file is only RGB
// Feel free to adjust this if you so please, by changing the 4 to a 0.
bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int&y)
{
    int n;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
    if (data != nullptr)
    {
        image = std::vector<unsigned char>(data, data + x * y * 4);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

int main()
{
    // -------------------------------------> Image Reading
    std::string filename = "image_skull.png";
    
    int width, height;
    std::vector<unsigned char> image;
    bool success = load_image(image, filename, width, height);
    if (!success)
    {
        std::cout << "Error loading image\n";
        return 1;
    }
    
    std::cout << "Image width = " << width << '\n';
    std::cout << "Image height = " << height << '\n';
    
    const size_t RGB = 3;

    unsigned char* data = &image.front();

    stbi_write_png("output.png", width, height, 4, data, width*4);

    return 0;
}