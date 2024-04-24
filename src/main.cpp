#include <iostream>
#include <vector>

#include <cfloat>
#include <chrono>
#include <cstdio>
#include <functional>
#include <random>
#include <vector>

// OpenMP
#include <omp.h>

#define NUM_KMEANS 256
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "../include/stb_image_write.h"

#include "ColourQuantization/main.hpp"

struct Vector3D {
    int x, y, z;
};

int run(const std::string &FileName) {
    // Image Reading
    int width, height;
    std::vector<unsigned char> image_data;

    bool success = load_image(image_data, FileName, width, height);
    if (!success) {
        std::cout << "Error loading image\n";
        return 1;
    }

    std::vector<int> image(3 * width * height);
    for (int i = 0; i < 3 * width * height; i++) {
        image[i] = static_cast<int>(image_data[i]);
    }

    std::cout << "Image width = " << width << '\n';
    std::cout << "Image height = " << height << '\n';

    // Create tag vector
    std::vector<int> tags(width * height, -1);

    // generate random k means
    std::vector<std::vector<int>> kmeans = select_random(image, width, height);

    for (int k = 0; k < 5; k++) {

        tags = find_distance(width, height, kmeans, image, tags);
        kmeans = tag_pixels(width, height, kmeans, image, tags);
    }

    modify_and_print(width, height, kmeans, image_data, tags);

    return 0;
}

std::vector<std::vector<int>> tag_pixels(int width, int height,
                                         std::vector<std::vector<int>> kmeans,
                                         std::vector<int> image,
                                         std::vector<int> tags) {
    std::vector<std::vector<int>> modifiedKmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
        int num = 0;
        double red = 0;
        double green = 0;
        double blue = 0;
        int x, y;
        // #pragma omp parallel for reduction(+:red,green,blue) private(x)
        // num_threads(4)
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                size_t index = 3 * (x + y * width);
                if (tags[x + y * width] == i) {
                    red += image[index + 0];
                    green += image[index + 1];
                    blue += image[index + 2];
                    num += 1;
                }
            }
        }
        if (num != 0) {
            red /= num;
            green /= num;
            blue /= num;
        } else {
            // If there are no matches we set it to the original value
            red = kmeans[i][0];
            green = kmeans[i][1];
            blue = kmeans[i][2];
        }
        modifiedKmeans[i] =
            std::vector<int>{static_cast<int>(red), static_cast<int>(green),
                             static_cast<int>(blue), i};
    }
    return modifiedKmeans;
}

double calculateDistance(const Vector3D &v1, const Vector3D &v2) {
    return (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) +
           (v1.z - v2.z) * (v1.z - v2.z);
}

int findClosestCluster(const Vector3D &vector,
                       const std::vector<std::vector<int>> &kmeans) {
    int tag = -1;
    double minDistance = std::numeric_limits<double>::max();

    for (int i = 0; i < NUM_KMEANS; ++i) {
        Vector3D clusterCenter = {kmeans[i][0], kmeans[i][1], kmeans[i][2]};
        double distance = calculateDistance(vector, clusterCenter);
        if (distance < minDistance) {
            minDistance = distance;
            tag = i;
        }
    }

    return tag;
}

std::vector<int> find_distance(int width, int height,
                               std::vector<std::vector<int>> kmeans,
                               std::vector<int> image, std::vector<int> tags) {
    std::vector<int> closestCenters(image.size() / 3);

    size_t i;
    // #pragma omp parallel for private(i) num_threads(4)
    for (i = 0; i < image.size(); i += 3) {
        Vector3D vector = {image[i], image[i + 1], image[i + 2]};
        int closestCenter = findClosestCluster(vector, kmeans);

        closestCenters[i / 3] = closestCenter;
    }
    return closestCenters;
}

// Loads as RGBA... even if file is only RGB
// Feel free to adjust this if you so please, by changing the 4 to a 0.
bool load_image(std::vector<unsigned char> &image, const std::string &filename,
                int &x, int &y) {
    int n;
    unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 3);
    if (data != nullptr) {
        image = std::vector<unsigned char>(data, data + x * y * 3);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

void modify_and_print(int width, int height,
                      std::vector<std::vector<int>> kmeans,
                      std::vector<unsigned char> image_data,
                      std::vector<int> tags) {
    // -----------------------> Modify image data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t index = 3 * (x + y * width);
            image_data[index + 0] =
                static_cast<unsigned char>(kmeans[tags[x + y * width]][0]);
            image_data[index + 1] =
                static_cast<unsigned char>(kmeans[tags[x + y * width]][1]);
            image_data[index + 2] =
                static_cast<unsigned char>(kmeans[tags[x + y * width]][2]);
        }
    }

    unsigned char *data = &image_data.front();

    remove("output.png");
    stbi_write_png("output.png", width, height, 3, data, width * 3);
}

std::vector<std::vector<int>> select_random(std::vector<int> image, int width,
                                            int height) {

    // generate a random seed
    std::random_device r;
    std::seed_seq seed_seq{r(), r(), r(), r(), r(), r(), r(), r()};
    long unsigned int consantSeed = 42;

    // initialise the rng with the seed
    std::mt19937 mersenne{consantSeed};

    std::uniform_int_distribution<int> dice(0, width * height);

    // times three to get the start of a pixel RGB, our generator only does
    // width*height
    std::vector<std::vector<int>> kmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
        int index = dice(mersenne) * 3;
        kmeans[i] = std::vector<int>{image[index + 0], image[index + 1],
                                     image[index + 2], i};
    }
    return kmeans;
}

std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image, int width,
                                    int height) {
    // generate a random seed
    std::random_device r;
    std::seed_seq seed_seq{r(), r(), r(), r(), r(), r(), r(), r()};

    // initialise the rng with the seed
    std::mt19937 mersenne{seed_seq};

    std::uniform_int_distribution<int> dice(0, NUM_KMEANS - 1);

    std::vector<int> tags(width * height, -1);

    // randomly assign a tag
    for (int i = 0; i < width * height; i++) {
        tags[i] = dice(mersenne);
    }

    std::vector<std::vector<int>> kmeans(NUM_KMEANS);

    int num = 0;
    for (int i = 0; i < NUM_KMEANS; i++) {
        std::vector<double> mean{0.0, 0.0, 0.0};
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                size_t index = 3 * (y * width + x);
                if (tags[y * height + x] == i) {
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

        kmeans[i] = std::vector<int>{static_cast<int>(mean[0]),
                                     static_cast<int>(mean[1]),
                                     static_cast<int>(mean[2]), i};
    }

    return kmeans;
}

std::vector<std::vector<int>> complete_random() {

    // generate a random seed
    std::random_device r;
    std::seed_seq seed_seq{r(), r(), r(), r(), r(), r(), r(), r()};

    // initialise the rng with the seed
    std::mt19937 mersenne{seed_seq};

    std::uniform_int_distribution<int> dice(0, 255);

    std::vector<std::vector<int>> kmeans(NUM_KMEANS);
    for (int i = 0; i < NUM_KMEANS; i++) {
        kmeans[i] =
            std::vector<int>{dice(mersenne), dice(mersenne), dice(mersenne), i};
        printf("Vector %d is {%d, %d, %d}\n", kmeans[i][3], kmeans[i][0],
               kmeans[i][1], kmeans[i][2]);
    }

    return kmeans;
}
