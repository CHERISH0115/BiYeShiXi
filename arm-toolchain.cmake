set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# Allow finding headers in both ARM sysroot and host system
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

set(CMAKE_LIBRARY_PATH /usr/lib/arm-linux-gnueabihf)

# Manually set JPEG paths for ARM
set(JPEG_INCLUDE_DIR /usr/include)
set(JPEG_LIBRARY /usr/lib/arm-linux-gnueabihf/libjpeg.so)
