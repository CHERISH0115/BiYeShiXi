#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <setjmp.h>

#define WIDTH 640
#define HEIGHT 480

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

void my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

void yuyv_to_rgb(const unsigned char *yuyv, unsigned char *rgb, int width, int height) {
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j += 2) {
            int y0 = yuyv[i * width * 2 + j * 2];
            int u = yuyv[i * width * 2 + j * 2 + 1] - 128;
            int y1 = yuyv[i * width * 2 + j * 2 + 2];
            int v = yuyv[i * width * 2 + j * 2 + 3] - 128;

            int r, g, b;

            r = y0 + ((351 * v) >> 8);
            g = y0 - ((179 * v + 86 * u) >> 8);
            b = y0 + ((443 * u) >> 8);

            if (r > 255) r = 255;
            if (r < 0) r = 0;
            if (g > 255) g = 255;
            if (g < 0) g = 0;
            if (b > 255) b = 255;
            if (b < 0) b = 0;

            rgb[i * width * 3 + j * 3] = r;
            rgb[i * width * 3 + j * 3 + 1] = g;
            rgb[i * width * 3 + j * 3 + 2] = b;

            r = y1 + ((351 * v) >> 8);
            g = y1 - ((179 * v + 86 * u) >> 8);
            b = y1 + ((443 * u) >> 8);

            if (r > 255) r = 255;
            if (r < 0) r = 0;
            if (g > 255) g = 255;
            if (g < 0) g = 0;
            if (b > 255) b = 255;
            if (b < 0) b = 0;

            rgb[i * width * 3 + j * 3 + 3] = r;
            rgb[i * width * 3 + j * 3 + 4] = g;
            rgb[i * width * 3 + j * 3 + 5] = b;
        }
    }
}

int write_jpeg(const char *filename, unsigned char *rgb, int width, int height, int quality) {
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    FILE *outfile;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
        return 1;
    }

    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Could not open %s\n", filename);
        return 1;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    row_stride = width * 3;

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &rgb[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_yuv> <output_jpg>\n", argv[0]);
        fprintf(stderr, "Example: %s input.yuv output.jpg\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    FILE *fp = fopen(input_file, "rb");
    if (!fp) {
        perror("Cannot open input file");
        return 1;
    }

    size_t yuv_size = WIDTH * HEIGHT * 2;
    unsigned char *yuv_buffer = malloc(yuv_size);
    if (!yuv_buffer) {
        perror("Out of memory");
        fclose(fp);
        return 1;
    }

    size_t read_size = fread(yuv_buffer, 1, yuv_size, fp);
    fclose(fp);

    if (read_size != yuv_size) {
        fprintf(stderr, "Input file size mismatch. Expected %zu bytes, got %zu bytes\n", yuv_size, read_size);
        free(yuv_buffer);
        return 1;
    }

    size_t rgb_size = WIDTH * HEIGHT * 3;
    unsigned char *rgb_buffer = malloc(rgb_size);
    if (!rgb_buffer) {
        perror("Out of memory");
        free(yuv_buffer);
        return 1;
    }

    yuyv_to_rgb(yuv_buffer, rgb_buffer, WIDTH, HEIGHT);
    free(yuv_buffer);

    if (write_jpeg(output_file, rgb_buffer, WIDTH, HEIGHT, 90) != 0) {
        fprintf(stderr, "Failed to write JPEG file\n");
        free(rgb_buffer);
        return 1;
    }

    free(rgb_buffer);

    printf("Converted %s to %s (%dx%d)\n", input_file, output_file, WIDTH, HEIGHT);

    return 0;
}
