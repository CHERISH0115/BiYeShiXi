#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {
    const char *dev = argc > 1 ? argv[1] : "/dev/video0";
    
    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    printf("Device opened: %s (fd=%d)\n", dev, fd);
    
    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));
    
    printf("Calling VIDIOC_QUERYCAP...\n");
    printf("sizeof(cap) = %zu\n", sizeof(cap));
    
    int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        perror("VIDIOC_QUERYCAP");
        printf("errno = %d\n", errno);
        close(fd);
        return 1;
    }
    
    printf("Driver: %s\n", cap.driver);
    printf("Card: %s\n", cap.card);
    printf("Bus info: %s\n", cap.bus_info);
    printf("Version: %u.%u.%u\n", 
           (cap.version >> 16) & 0xFF,
           (cap.version >> 8) & 0xFF,
           cap.version & 0xFF);
    printf("Capabilities: 0x%08x\n", cap.capabilities);
    
    close(fd);
    return 0;
}
