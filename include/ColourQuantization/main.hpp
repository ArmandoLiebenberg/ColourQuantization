#pragma once

#include <vector>
#include <string>

void modify_and_print(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<unsigned char> image, std::vector<int> tags);
std::vector<std::vector<int>> tag_pixels(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<int>  image, std::vector<int> tags);
std::vector<int> find_distance(int width, int height, std::vector<std::vector<int>> kmeans, std::vector<int>  image, std::vector<int> tags);
bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int&y);
std::vector<std::vector<int>> complete_random();
std::vector<std::vector<int>> select_random(std::vector<int>  image, int width, int height);
std::vector<std::vector<int>> fuzzy(std::vector<unsigned char> image, int width, int height);
int run(const std::string& FileName);