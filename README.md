
<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

![mountains](https://github.com/ArmandoLiebenberg/ColourQuantization/assets/34469720/5a2d9f22-0b5d-467e-929a-7ea5d686846c) ![image](https://github.com/ArmandoLiebenberg/ColourQuantization/assets/34469720/9cbdd446-2452-4354-aa0c-4ee28034f0b8)




Colour Image Quantization using K-Means Clustering. Color quantization is quantization (Compressing a range of values) applied to color spaces, typically of images or video.
In practice this means taking an image, sampling its colour palette, and producing a smaller color pallete that maintains the detail and brightness of the original image.

This is typically done through means of a clustering algorithm. Treating the colours in the color pallete as vectors in 3D space (Typically in RGB, though there are color spaces
that are more suited for this sort of thing), and solving using a clustering algorithm. For this problem we've gone with the simple 'Naive K-means' clustering algorithm.

### Built With

The project is written in C++, using make to build. For image processing we use the wonderful stb_image and stb_image_write libraries from Sean Barrett.

<!-- GETTING STARTED -->
## Getting Started

Simply build with make, and run the executable

### Prerequisites

Gnu Make 4.2.1 is used
G++ 8.5.0 20210514 is used

<!-- USAGE EXAMPLES -->
## Usage

Run with ./make

<!-- ROADMAP -->
## Roadmap

- [ ] Add support for compiling to windows .exe
- [ ] Add command line options for different initialization methods
- [ ] Add command line options for color pallete size
- [ ] Use SIMD to speed up
- [ ] Add support for OPENMP/MPI over server cluster
- [ ] Add GUI support using QT or Dear IMGUI

See the [open issues](https://github.com/ArmandoLiebenberg/ColourQuantization/issues) for a full list of proposed features (and known issues).

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<!-- CONTACT -->
## Contact

Armando Liebenberg - armandoliebenberg@gmail.com

Project Link: [https://github.com/ArmandoLiebenberg/ColourQuantization](https://github.com/ArmandoLiebenberg/ColourQuantization)

<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Improving the Performance of K-Means for Color Quantization](https://arxiv.org/abs/1101.0395)
* [stb single-file public domain libraries for C/C++ ](https://github.com/nothings/stb)
