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

#define NUM_KMEANS 3
#define STB_IMAGE_WRITE_IMPLEMENTATION

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "../lib/stb_image.h"
    #include "../lib/stb_image_write.h"
}

bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int&y);
std::vector<std::vector<int>> complete_random();
std::vector<std::vector<int>> select_random(std::vector<unsigned char> image, int width, int height);
std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image, int width, int height);


int main()
{
    // -------------------------------------> Image Reading
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
    
    const size_t RGB = 3;

    // -------------------------------------> Create tag vector
    std::vector<int> tags(width*height, -1);

    // -------------------------------------> generate random k means 

    //std::vector<std::vector<int>> kmeans = complete_random();
    std::vector<std::vector<int>> kmeans = select_random(image, width, height);
    //std::vector<std::vector<int>> kmeans = fuzzy(image, width, height);

/*
    bool changed = true;
    for (int k = 0; k < 1; k++) {
        changed = false;

        // For each pixel, assign to nearest cluster
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
                tags[x + y*width] = tag;
            }
        }

        // Recompute the center of each cluster
        for (int kmean = 0; kmean < NUM_KMEANS; kmean++) {

            std::vector<double> mean {0.0, 0.0, 0.0};
            int num = 0;

            for(int pixel = 0; pixel < width*height; pixel++) {
                if (tags[pixel] == kmean) {
                    //printf("pixel data: %d, %d, %d\n", static_cast<int>(image[pixel + 0]), static_cast<int>(image[pixel + 1]), static_cast<int>(image[pixel + 2]));
                    mean[0] += static_cast<int>(image[pixel + 0]);
                    mean[1] += static_cast<int>(image[pixel + 1]);
                    mean[2] += static_cast<int>(image[pixel + 2]);
                    num += 1;
                }
            }

            // find average. If no pixels, leave as orignal value
            
            if (num > 0) {
                mean[0] = mean[0] / num;
                mean[1] = mean[1] / num;
                mean[2] = mean[2] / num;
                printf("cluser: %d  rgb: (%g, %g, %g)\n", kmean, mean[0], mean[1], mean[2]);
                kmeans[kmean] = std::vector<int>{static_cast<int>(std::floor(mean[0])), static_cast<int>(std::floor(mean[1])), static_cast<int>(std::floor(mean[2])), kmean};
            }
        }
    }*/

    int count = 0;
    while (count < 15) {
        //printf("--------- ITERATION %d ----------", count);
        // for each vector:
         // For each pixel, assign to nearest cluster
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
                tags[x + y*width] = tag;
            }
        }

        // for each set of vectors belonging to a tag
        // calculate a new mean based on the average location of those vectors
        
        for (int i = 0; i < NUM_KMEANS; i++) {
            int num = 0;
            std::vector<double> mean {0.0, 0.0, 0.0};
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    size_t index = RGB * (x + y * width);
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
            kmeans[i] = std::vector<int>{static_cast<int>(mean[0]), static_cast<int>(mean[1]), static_cast<int>(mean[2]), i};

        }
        
        count += 1;
    }





    // -----------------------> Modify image data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t index = RGB * (x + y*width);
            image[index + 0] = static_cast<unsigned char>(kmeans[tags[x + y*width]][0]);
            image[index + 1] = static_cast<unsigned char>(kmeans[tags[x + y*width]][1]);
            image[index + 2] = static_cast<unsigned char>(kmeans[tags[x + y*width]][2]);

        }
    }

    // print all the tags
    /*
    for (int i = 0; i < width*height; i++) {
        printf("%d ", tags[i]);
    }
    printf("\n");*/

    unsigned char* data = &image.front();

    remove("output.png");
    stbi_write_png("output.png", width, height, RGB, data, width*RGB);

    return 0;
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

std::vector<std::vector<int>> select_random(std::vector<unsigned char> image, int width, int height) 
{
    // ------- generate random k means
    /*typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration time = myclock::now() - beginning;
    unsigned seed = time.count();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0,width*height);
    auto dice = std::bind ( distribution, generator );*/

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
        printf("Index is: %d\n", index/2);
        kmeans[i] = std::vector<int> {static_cast<int>(image[index + 0]), static_cast<int>(image[index + 1]), static_cast<int>(image[index + 2]), i};
    }
    return kmeans;
}

std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image, int width, int height)
{
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
