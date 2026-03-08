#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <errno.h>

#define WIDTH 640
#define HEIGHT 480
#define BUFFER_COUNT 4

struct buffer {
    void *start;
    size_t length;
};

static int xioctl(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (r == -1 && EINTR == errno);
    return r;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <device> <output_file>\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/video0 output.yuv\n", argv[0]);
        return 1;
    }

    const char *dev_name = argv[1];
    const char *output_file = argv[2];

    int fd = open(dev_name, O_RDWR, 0);
    if (fd < 0) {
        perror("Cannot open device");
        return 1;
    }

    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        close(fd);
        return 1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "Device is not a video capture device\n");
        close(fd);
        return 1;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "Device does not support streaming\n");
        close(fd);
        return 1;
    }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        fprintf(stderr, "Device does not support YUYV format\n");
        close(fd);
        return 1;
    }

    if (fmt.fmt.pix.width != WIDTH || fmt.fmt.pix.height != HEIGHT) {
        fprintf(stderr, "Warning: Driver is using resolution %dx%d instead of %dx%d\n",
                fmt.fmt.pix.width, fmt.fmt.pix.height, WIDTH, HEIGHT);
    }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        close(fd);
        return 1;
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory\n");
        close(fd);
        return 1;
    }

    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        perror("Out of memory");
        close(fd);
        return 1;
    }

    for (unsigned int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            free(buffers);
            close(fd);
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length,
                                PROT_READ | PROT_WRITE, MAP_SHARED,
                                fd, buf.m.offset);

        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            free(buffers);
            close(fd);
            return 1;
        }
    }

    for (unsigned int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            free(buffers);
            close(fd);
            return 1;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        free(buffers);
        close(fd);
        return 1;
    }

    fd_set fds;
    struct timeval tv;
    int r;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    r = select(fd + 1, &fds, NULL, NULL, &tv);
    if (r < 0) {
        perror("select");
        xioctl(fd, VIDIOC_STREAMOFF, &type);
        free(buffers);
        close(fd);
        return 1;
    }
    if (r == 0) {
        fprintf(stderr, "Timeout waiting for frame\n");
        xioctl(fd, VIDIOC_STREAMOFF, &type);
        free(buffers);
        close(fd);
        return 1;
    }

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        perror("VIDIOC_DQBUF");
        xioctl(fd, VIDIOC_STREAMOFF, &type);
        free(buffers);
        close(fd);
        return 1;
    }

    FILE *fp = fopen(output_file, "wb");
    if (!fp) {
        perror("Cannot open output file");
        xioctl(fd, VIDIOC_STREAMOFF, &type);
        free(buffers);
        close(fd);
        return 1;
    }

    fwrite(buffers[buf.index].start, buf.bytesused, 1, fp);
    fclose(fp);

    printf("Captured frame saved to %s (%dx%d, YUYV format)\n",
           output_file, fmt.fmt.pix.width, fmt.fmt.pix.height);

    if (xioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        perror("VIDIOC_QBUF");
    }

    xioctl(fd, VIDIOC_STREAMOFF, &type);

    for (unsigned int i = 0; i < req.count; i++) {
        munmap(buffers[i].start, buffers[i].length);
    }

    free(buffers);
    close(fd);

    return 0;
}
