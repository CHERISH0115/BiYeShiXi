cmake_minimum_required(VERSION 3.10)
project(camera_capture)

set(CMAKE_C_STANDARD 11)

# Camera capture executable
add_executable(camera_capture main.c)

find_library(V4L2_LIBRARY v4l2)
if(V4L2_LIBRARY)
    target_link_libraries(camera_capture ${V4L2_LIBRARY})
endif()

# YUV to JPEG converter executable
add_executable(yuv_to_jpeg yuv_to_jpeg.c)

find_package(JPEG REQUIRED)
if(JPEG_FOUND)
    target_include_directories(yuv_to_jpeg PRIVATE ${JPEG_INCLUDE_DIR})
    target_link_libraries(yuv_to_jpeg ${JPEG_LIBRARIES})
endif()
