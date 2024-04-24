file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ColourImageQuantization_SOURCE_DIR}/include/ColourQuantization/*.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(quantization_library lib.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(quantization_library PUBLIC ../include)

# All users of this library will need at least C++11
target_compile_features(quantization_library PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
