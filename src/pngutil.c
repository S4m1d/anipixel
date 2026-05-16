#include "pngutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

int pngutil_load_png(const char *path, PngutilImage *img) {
    FILE *fp = fopen(path, "rb");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", path); return 0; }

    // validate PNG signature
    unsigned char sig[8];
    fread(sig, 1, 8, fp);
    if (!png_check_sig(sig, 8)) {
        fprintf(stderr, "Not a PNG file\n");
        fclose(fp);
        return 1;
    }

    png_structp png  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop   info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error reading PNG\n");
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 1;
    }

    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);   // we already read 8 bytes
    png_read_info(png, info);

    img->width  = png_get_image_width(png, info);
    img->height = png_get_image_height(png, info);

    // normalize everything to 8-bit RGBA
    png_set_expand(png);          // expand palette, grayscale, tRNS to full color
    png_set_strip_16(png);        // 16-bit down to 8-bit
    png_set_gray_to_rgb(png);     // grayscale to RGB
    png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);  // add alpha if missing
    png_read_update_info(png, info);

    // allocate row pointers
    img->pixels = malloc(img->height * sizeof(unsigned char *));
    for (int y = 0; y < img->height; y++)
        img->pixels[y] = malloc(img->width * 4);  // 4 bytes per pixel (RGBA)

    png_read_image(png, img->pixels);

    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return 0;
}

void pngutil_free_image(PngutilImage *img) {
    for (int y = 0; y < img->height; y++)
        free(img->pixels[y]);
    free(img->pixels);
}
