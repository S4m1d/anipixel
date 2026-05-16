#include "pxconv.h"
#include "pngutil.h"
#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PxconvError pxconv_png_to_ssd1306(PngutilImage *img,
                                  uint8_t frame_bytes[8][128]) {
  if (img->height != 64 || img->width != 128) {
    fprintf(stderr,
            "SSD1306 format expects 128x64 resolution, while the passed image "
            "has %dx%d\n",
            img->width, img->height);
    return PXCONV_ERR;
  }

  // going through ssd1306 pages
  for (int y = 0; y < 8; y++) {

    // going through ssd1306 rows
    for (int x = 0; x < 128; x++) {
      uint8_t page = 0x00;

      // for each ssd1306 page collect 8 png's pixels
      for (int offset = 0; offset < 8; offset++) {

        // check Alpha channel of each pixel, if non transparent, set pixel bit
        // to page
        if (img->pixels[y * 8 + offset][x * 4 + 3] != 0) {
          page |= 0x01 << offset;
        }
      }

      frame_bytes[y][x] = page;
    }
  }
  return PXCONV_OK;
}

PxconvError
pxconv_pick_ssd1306_frame_from_sprites(PngutilImage *sprites_img, int start_x,
                                       int start_y,
                                       uint8_t frame_bytes[8][128]) {
  // going through ssd1306 pages
  for (int y = 0; y < 8; y++) {

    // going through ssd1306 rows
    for (int x = 0; x < 128; x++) {
      uint8_t page = 0x00;

      // for each ssd1306 page collect 8 png's pixels
      for (int offset = 0; offset < 8; offset++) {

        // check Alpha channel of each pixel, if non transparent, set pixel bit
        // to page
        if (sprites_img->pixels[start_y + y * 8 + offset]
                               [(start_x + x) * 4 + 3] != 0) {
          page |= 0x01 << offset;
        }
      }

      frame_bytes[y][x] = page;
    }
  }
  return PXCONV_OK;
}

PxconvError pxconv_spritesheet_to_ssd1306(PngutilImage *img, cJSON *meta,
                                          uint8_t **animation_bytes) {
  cJSON *frames = cJSON_GetObjectItem(meta, "frames");
  int frames_count = cJSON_GetArraySize(frames);
  uint8_t *buf = malloc((sizeof(int) + 1024 * sizeof(uint8_t)) * frames_count);
  if (!buf) {
    fprintf(stderr,
            "pxconv_spritesheet_to_ssd1306: failed to allocate buffer\n");
    return PXCONV_ERR;
  }

  for (int i = 0; i < frames_count; i++) {
    cJSON *params = cJSON_GetArrayItem(frames, i);
    cJSON *frame = cJSON_GetObjectItem(params, "frame");

    int x = cJSON_GetObjectItem(frame, "x")->valueint;
    int y = cJSON_GetObjectItem(frame, "y")->valueint;
    int w = cJSON_GetObjectItem(frame, "w")->valueint;
    int h = cJSON_GetObjectItem(frame, "h")->valueint;

    if (w > 128 || h > 64) {
      fprintf(stderr,
              "pxconv_spritesheet_to_ssd1306: SSD1306 format expects 128x64 "
              "resolution, while the frame "
              "has %dx%d\n",
              w, h);
      return PXCONV_ERR;
    }

    int offset = i * (sizeof(int) + 1024 * sizeof(uint8_t));

    int duration_ms = cJSON_GetObjectItem(params, "duration")->valueint;
    // writing duration of frame first
    memcpy(buf + offset, &duration_ms, sizeof(int));

    uint8_t frame_bytes[8][128];
    pxconv_pick_ssd1306_frame_from_sprites(img, x, y, frame_bytes);
    // then write pages data itself
    memcpy(buf + offset + sizeof(int), frame_bytes, sizeof(uint8_t) * 1024);
  }

  *animation_bytes = buf;
  return PXCONV_OK;
}
