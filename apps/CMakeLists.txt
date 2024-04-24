add_executable(app app.cpp ${HEADER_LIST})

target_compile_features(app PRIVATE cxx_std_17)

target_link_libraries(app PRIVATE quantization_library)