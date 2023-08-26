//
// Created by aliebs on 11/08/23.
//

#define NUM_KMEANS 3
#define NUM_VECTORS 4
#define FAR 99999

#include <cstdio>
#include<vector>
#include<random>
#include <functional>
#include <chrono>

int main(int argc, char* argv[]) {
    printf("Hello, world!\n");

    // read in image file using the library
    // store image file into array of vectors, with tags
    std::vector<int> myVectors[NUM_VECTORS] =
            {std::vector<int> {100, 255, 175, -1}, std::vector<int> {60, 0, 15, -1},
            std::vector<int> {25, 100, 200, -1}, std::vector<int> {255, 55, 10, -1}};

    // ------- generate random k means
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration time = myclock::now() - beginning;
    unsigned seed = time.count();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(1,255);
    auto dice = std::bind ( distribution, generator );

    std::vector<int> kmeans[NUM_KMEANS];
    for (int i = 0; i < NUM_KMEANS; i++) {
        kmeans[i] = std::vector<int> {dice(), dice(), dice(), i};
        printf("Vector %d is {%d, %d, %d}\n", kmeans[i][3], kmeans[i][0], kmeans[i][1], kmeans[i][2]);
    }

    // for each vector:
    for (auto & myVector : myVectors) {
        printf("%d\n",myVector[0]);

        // find the closest k means
        double longestDistance = FAR;
        int longestTag = -1;
        for (auto & kmean : kmeans) {
            double distance = std::sqrt(pow((kmean[0] - myVector[0]),2) + pow((kmean[1] - myVector[1]),2) + pow((kmean[2] - myVector[2]),2));
            if (distance < longestDistance) {
                longestDistance = distance;
                longestTag = kmean[3];
            }
        }
        // tag the vector with that k means tag
        if (longestDistance != FAR) {
            myVector[3] = longestTag;
            printf("Closest kmeans of vector is %d\n", myVector[3]);
        }
    }
    // for each set of vectors belonging to a tag
        // calculate a new mean based on the average location of those vectors

    return 0;
}
