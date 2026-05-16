#ifndef PXCONV_H
#define PXCONV_H
#include "cjson/cJSON.h"
#include "pngutil.h"
#include <stdint.h>

typedef enum {
  PXCONV_OK = 0,
  PXCONV_ERR,
} PxconvError;

PxconvError pxconv_png_to_ssd1306(PngutilImage *img,
                                  uint8_t frame_bytes[8][128]);

PxconvError pxconv_spritesheet_to_ssd1306(PngutilImage *img, cJSON *meta,
                                          uint8_t **animation_bytes,
                                          int *animation_size);
#endif
