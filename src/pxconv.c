#include "pxconv.h"
#include "png.h"
#include "pngutil.h"
#include "unistd.h"
#include <stdint.h>
#include <stdio.h>

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
