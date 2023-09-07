//
// Created by aliebs on 11/08/23.
//

#include <iostream>
#include <vector>

#include <cstdio>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <cfloat>

#define NUM_KMEANS 256
#define STB_IMAGE_WRITE_IMPLEMENTATION

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "../lib/stb_image.h"
    #include "../lib/stb_image_write.h"
}

void modify_and_print(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<unsigned char> image, std::vector<int> tags);
std::vector<std::vector<int>> tag_pixels(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<unsigned char> image, std::vector<int> tags);
std::vector<int> find_distance(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<unsigned char> image, std::vector<int> tags);
bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int&y);
std::vector<std::vector<int>> complete_random();
std::vector<std::vector<int>> select_random(std::vector<unsigned char> image, int width, int height);
std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image, int width, int height);



int main()
{
    // Image Reading
    std::string filename = "mountains.png";
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

    // Create tag vector
    std::vector<int> tags(width*height, -1);

    // generate random k means 
    std::vector<std::vector<int>> kmeans = select_random(image, width, height);
    //std::vector<std::vector<int>> kmeans = complete_random();
    //std::vector<std::vector<int>> kmeans = fuzzy(image, width, height);

    int cycles = 0;
    bool changed = true;
    while (changed) {
        changed = false;

        tags = find_distance(width, height, kmeans, image, tags);
        std::vector<std::vector<int>> newKmeans = tag_pixels(width, height, kmeans, image, tags);

        if (kmeans != newKmeans) {
            changed = true;
            kmeans = newKmeans;
            cycles += 1;
        }
    }

    modify_and_print(width, height, kmeans, image, tags);

    printf("%d cycles ran\n", cycles);

    return 0;
}

void modify_and_print(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<unsigned char> image, std::vector<int> tags) {
    // -----------------------> Modify image data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t index = 3 * (x + y*width);
            image[index + 0] = static_cast<unsigned char>(kmeans[tags[x + y*width]][0]);
            image[index + 1] = static_cast<unsigned char>(kmeans[tags[x + y*width]][1]);
            image[index + 2] = static_cast<unsigned char>(kmeans[tags[x + y*width]][2]);

        }
    }

    unsigned char* data = &image.front();

    remove("output.png");
    stbi_write_png("output.png", width, height, 3, data, width*3);
}

std::vector<std::vector<int>> tag_pixels(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<unsigned char> image, std::vector<int> tags) {
    std::vector<std::vector<int>> modifiedKmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
            int num = 0;
            std::vector<double> mean {0.0, 0.0, 0.0};
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    size_t index = 3 * (x + y * width);
                    if (tags[x + y*width] == i) {
                        mean[0] += static_cast<int>(image[index + 0]);
                        mean[1] += static_cast<int>(image[index + 1]);
                        mean[2] += static_cast<int>(image[index + 2]);
                        num += 1;
                    }
                }
            }
            if (num != 0) {
                mean[0] /= num;
                mean[1] /= num;
                mean[2] /= num;
            } else {
                // If there are no matches we set it to the original value
                mean[0] = kmeans[i][0];
                mean[1] = kmeans[i][1];
                mean[2] = kmeans[i][2];
            }
            modifiedKmeans[i] = std::vector<int>{static_cast<int>(mean[0]), static_cast<int>(mean[1]), static_cast<int>(mean[2]), i};
    }
    return modifiedKmeans;
}

std::vector<int> find_distance(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<unsigned char> image, std::vector<int> tags) {
    std::vector<int> modifiedTags(width*height, -1);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t index = 3 * (x + y*width); // data is sequential in memory

            double miniumum_distance = DBL_MAX;
            int tag = -1;

            for (int i = 0; i < NUM_KMEANS; i++) {
                double r = kmeans[i][0] - static_cast<int>(image[index + 0]);
                double g = kmeans[i][1] - static_cast<int>(image[index + 1]);
                double b = kmeans[i][2] - static_cast<int>(image[index + 2]);
                double distance = std::sqrt(r*r + g*g + b*b);

                if (distance < miniumum_distance) {
                    miniumum_distance = distance;
                    tag = i;
                }
            }

            //printf("pixel: %d  tag: %d\n", x + y*width, tag);
            modifiedTags[x + y*width] = tag;
        }
    }
    return modifiedTags;
}

// Loads as RGBA... even if file is only RGB
// Feel free to adjust this if you so please, by changing the 4 to a 0.
bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int&y)
{
    int n;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 3);
    if (data != nullptr)
    {
        image = std::vector<unsigned char>(data, data + x * y * 3);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

std::vector<std::vector<int>> complete_random() 
{

    // generate a random seed
    std::random_device r;
    std::seed_seq seed_seq{r(), r(), r(), r(), r(), r(), r(), r()};

    // initialise the rng with the seed
    std::mt19937 mersenne{seed_seq};

    std::uniform_int_distribution<int> dice(0, 255);

    std::vector<std::vector<int>> kmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
        kmeans[i] = std::vector<int> {dice(mersenne), dice(mersenne), dice(mersenne), i};
        printf("Vector %d is {%d, %d, %d}\n", kmeans[i][3], kmeans[i][0], kmeans[i][1], kmeans[i][2]);
    }

    return kmeans;
}

std::vector<std::vector<int>> select_random(std::vector<unsigned char> image, int width, int height) 
{

    // generate a random seed
    std::random_device r;
    std::seed_seq seed_seq{r(), r(), r(), r(), r(), r(), r(), r()};

    // initialise the rng with the seed
    std::mt19937 mersenne{seed_seq};

    std::uniform_int_distribution<int> dice(0, width*height);

    // times three to get the start of a pixel RGB, our generator only does width*height
    std::vector<std::vector<int>> kmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
        int index = dice(mersenne) * 3;
        kmeans[i] = std::vector<int> {static_cast<int>(image[index + 0]), static_cast<int>(image[index + 1]), static_cast<int>(image[index + 2]), i};
    }
    return kmeans;
}

std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image, int width, int height)
{
    // generate a random seed
    std::random_device r;
    std::seed_seq seed_seq{r(), r(), r(), r(), r(), r(), r(), r()};

    // initialise the rng with the seed
    std::mt19937 mersenne{seed_seq};

    std::uniform_int_distribution<int> dice(0, NUM_KMEANS-1);
    
    std::vector<int> tags(width*height, -1);

    // randomly assign a tag
    for (int i = 0; i < width*height; i++) {
        tags[i] = dice(mersenne);
    }

    std::vector<std::vector<int>> kmeans(NUM_KMEANS);
   
    int num = 0;
    for (int i = 0; i < NUM_KMEANS; i++) {
        std::vector<double> mean {0.0, 0.0, 0.0};
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                size_t index = 3 * (y * width + x);
                if (tags[y*height + x] == i) {
                    mean[0] += static_cast<int>(image[index + 0]);
                    mean[1] += static_cast<int>(image[index + 1]);
                    mean[2] += static_cast<int>(image[index + 2]);
                    num += 1;
                }
            }
        }

        if (num != 0) {
            mean[0] /= num;
            mean[1] /= num;
            mean[2] /= num;
        } 

        kmeans[i] = std::vector<int>{static_cast<int>(mean[0]), static_cast<int>(mean[1]), static_cast<int>(mean[2]), i};

    }

    return kmeans;
}
