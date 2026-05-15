#include "pngutil.h"
#include "pxconv.h"
#include "stdio.h"
#include "unistd.h"
#include <bits/getopt_core.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  char *t_value = NULL;
  char *d_value = NULL;
  int opt;

  while ((opt = getopt(argc, argv, "t:d:")) != -1) {
    switch (opt) {
    case 't':
      t_value = optarg;
      break;
    case 'd':
      d_value = optarg;
      break;
    case '?':
      fprintf(stderr, "Usage: %s -t <target> -d <destination>\n", argv[0]);
      return 1;
    }
  }

  if (!t_value || !d_value) {
    fprintf(stderr, "Both -t and -d flags are required\n");
    return 1;
  }

  fprintf(stdout, "Target: %s\n Destination:%s\n", t_value, d_value);

  PngutilImage img;
  pngutil_load_png(t_value, &img);

  uint8_t frame_bytes[8][128];
  PxconvError rc = pxconv_png_to_ssd1306(&img, frame_bytes);
  if (rc != PXCONV_OK) {
    pngutil_free_image(&img);
    return 1;
  }

  pngutil_free_image(&img);

  // open file for write, if doesn't exist create one
  FILE *dest_file = fopen(d_value, "wb");
  fwrite(frame_bytes, sizeof(frame_bytes), 1, dest_file);
  fclose(dest_file);

  return 0;
}
