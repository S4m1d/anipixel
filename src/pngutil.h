#ifndef PNGUTIL_H
#define PNGUTIL_H
typedef struct {
  unsigned char **pixels; // row pointers, rows[y][x*4] = R, +1=G, +2=B, +3=A
  int width;
  int height;
} PngutilImage;

int pngutil_load_png(const char *path, PngutilImage *img);

void pngutil_free_image(PngutilImage *img);

#endif
