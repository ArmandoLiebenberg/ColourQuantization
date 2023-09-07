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

#define NUM_KMEANS 8
#define FAR 99999
#define NUM_ITERATIONS 4

#define STB_IMAGE_WRITE_IMPLEMENTATION


extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "../lib/stb_image.h"
    #include "../lib/stb_image_write.h"
}

// Loads as RGBA... even if file is only RGB
// Feel free to adjust this if you so please, by changing the 4 to a 0.
bool load_image(uint8_t* image, const std::string& filename, int& x, int&y)
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


std::vector<std::vector<int>> complete_random() {

    // ------- generate random k means
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration time = myclock::now() - beginning;
    unsigned seed = time.count();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(1,255);
    auto dice = std::bind ( distribution, generator );

    std::vector<std::vector<int>> kmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
        kmeans[i] = std::vector<int> {dice(), dice(), dice(), i};
        printf("Vector %d is {%d, %d, %d}\n", kmeans[i][3], kmeans[i][0], kmeans[i][1], kmeans[i][2]);
    }

    return kmeans;
}

std::vector<std::vector<int>> select_random(std::vector<unsigned char> image, int width, int height) {
    // ------- generate random k means
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration time = myclock::now() - beginning;
    unsigned seed = time.count();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0,width*height);
    auto dice = std::bind ( distribution, generator );

    // times three to get the start of a pixel RGB, our generator only does width*height
    std::vector<std::vector<int>> kmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
        int index = dice() * 3;
        printf("Index is: %d\n", index);
        kmeans[i] = std::vector<int> {static_cast<int>(image[index + 0]), static_cast<int>(image[index + 0]), static_cast<int>(image[index + 0]), i};
    }
    return kmeans;
}

std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image, int width, int height) {
    // ------- generate random k means
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration time = myclock::now() - beginning;
    unsigned seed = time.count();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0,NUM_KMEANS-1);
    auto dice = std::bind ( distribution, generator );
    
    std::vector<int> tags(width*height, -1);

    // randomly assign a tag
    for (int i = 0; i < width*height; i++) {
        tags[i] = dice();
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

/*
std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image) {

    return kmeans;
}*/

int main()
{
    // -------------------------------------> Image Reading
    std::string filename = "image_skull.png";
    
    int width, height;
    uint8_t* image = new uint8_t[width * height * 3];
    bool success = load_image(image, filename, width, height);
    if (!success)
    {
        std::cout << "Error loading image\n";
        return 1;
    }
    
    std::cout << "Image width = " << width << '\n';
    std::cout << "Image height = " << height << '\n';
    
    const size_t RGB = 3;

    // -------------------------------------> Create tag vector
    std::vector<int> tags(width*height, -1);

    // ------- generate random k means
    //std::vector<std::vector<int>> kmeans = complete_random();
    //std::vector<std::vector<int>> kmeans = select_random(image, width, height);
    std::vector<std::vector<int>> kmeans = fuzzy(image, width, height);

    int count = 0;
    while (count < NUM_ITERATIONS) {
        //printf("--------- ITERATION %d ----------", count);
        // for each vector:
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                size_t index = RGB * (y * width + x);
                //printf("x:%d, y:%d, %d %d %d\n", x, y, static_cast<int>(image[index + 0]), static_cast<int>(image[index + 1]), static_cast<int>(image[index + 2]));

                // find the closest k means
                double longestDistance = FAR;
                int longestTag = -1;
                for (int i = 0; i < NUM_KMEANS; i++) {
                    double distance = std::sqrt(pow((kmeans[i][0] - static_cast<int>(image[index + 0])),2) + pow((kmeans[i][1] 
                        - static_cast<int>(image[index + 1])),2) + pow((kmeans[i][2] - static_cast<int>(image[index + 2])),2));
                    //printf("%g\n", distance);
                    if (distance < longestDistance) {
                        longestDistance = distance;
                        longestTag = kmeans[i][3];
                    }
                }
                // tag the vector with that k means tag
                if (longestDistance != FAR) {
                    tags[y*height + x] = longestTag;
                    //printf("Closest kmeans of vector is %d\n", tags[y*height + x]);
                }

            }
            
        }
        // for each set of vectors belonging to a tag
        // calculate a new mean based on the average location of those vectors
        int num = 0;
        for (int i = 0; i < NUM_KMEANS; i++) {
            std::vector<double> mean {0.0, 0.0, 0.0};
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    size_t index = RGB * (y * width + x);
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
            } else {
                // If there are no matches we set it to the original value
                mean[0] = kmeans[i][0];
                mean[1] = kmeans[i][1];
                mean[2] = kmeans[i][2];
            }
            kmeans[i] = std::vector<int>{static_cast<int>(mean[0]), static_cast<int>(mean[1]), static_cast<int>(mean[2]), i};

        }
        count += 1;
    }

    // Print all the tags
    for (int i = 0; i < width*height; i++) {
        //printf("%d ", tags[i]);
    }
    printf("\n");   

    stbi_write_jpg("output.hdr", width, height, 3, &image, 100);

    return 0;
}

