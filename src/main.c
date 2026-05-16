#include "pngutil.h"
#include "pxconv.h"
#include "stdio.h"
#include "txtutil.h"
#include "unistd.h"
#include <bits/getopt_core.h>
#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int png_to_ssd1306(char *t_value, char *d_value);
int spritesheet_to_ssd1306(char *t_value, char *d_value);

int main(int argc, char *argv[]) {
  char *m_value = NULL;
  char *t_value = NULL;
  char *d_value = NULL;
  int opt;

  while ((opt = getopt(argc, argv, "m:t:d:")) != -1) {
    switch (opt) {
    case 'm':
      m_value = optarg;
      break;
    case 't':
      t_value = optarg;
      break;
    case 'd':
      d_value = optarg;
      break;
    case '?':
      fprintf(stderr, "Usage: %s -t <mode> -t <target> -d <destination>\n",
              argv[0]);
      return 1;
    }
  }

  if (!m_value || !t_value || !d_value) {
    fprintf(stderr, "All of: -m, -t and -d flags are required\n");
    return 1;
  }

  fprintf(stdout, "Mode: %s\nTarget: %s\nDestination:%s\n", m_value, t_value,
          d_value);

  if (strcmp(m_value, "png_to_ssd1306") == 0) {
    return png_to_ssd1306(t_value, d_value);
  } else if (strcmp(m_value, "spritesheet_to_ssd1306") == 0) {
    return spritesheet_to_ssd1306(t_value, d_value);
  } else {
    fprintf(stderr,
            "Unknown mode %s\n Use one of: png_to_ssd1306, "
            "spritesheet_to_ssd1306\n",
            m_value);
    return 1;
  }
}

int png_to_ssd1306(char *t_value, char *d_value) {
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

int spritesheet_to_ssd1306(char *t_value, char *d_value) {
  PngutilImage sprites_png;
  char sprites_path[256];
  snprintf(sprites_path, sizeof(sprites_path), "%s/sprites.png", t_value);

  int rc = pngutil_load_png(sprites_path, &sprites_png);
  if (rc) {
    fprintf(stderr, "failed to load sprites file\n");
    return rc;
  }

  char meta_path[256];
  snprintf(meta_path, sizeof(meta_path), "%s/meta.json", t_value);
  char *meta_str = txtutil_read(meta_path);
  if (!meta_str) {
    pngutil_free_image(&sprites_png);
    fprintf(stderr, "failed to load meta json file\n");
    return 1;
  }

  cJSON *meta = cJSON_Parse(meta_str);
  free(meta_str);
  if (!meta) {
    pngutil_free_image(&sprites_png);
    fprintf(stderr, "failed to parse meta json\n");
    return 1;
  }

  uint8_t *animation_bytes;
  int animation_size;
  pxconv_spritesheet_to_ssd1306(&sprites_png, meta, &animation_bytes, &animation_size);

  // open file for write, if doesn't exist create one
  FILE *dest_file = fopen(d_value, "wb");
  if (!dest_file) {
    pngutil_free_image(&sprites_png);
    cJSON_Delete(meta);
    fprintf(stderr, "failed to open file %s for write\n", d_value);
    return 1;
  }
  fwrite(animation_bytes, sizeof(uint8_t), animation_size, dest_file);
  fprintf(stdout, "successfully written animation binary into %s, size %dB\n", d_value, animation_size);

  free(animation_bytes);
  fclose(dest_file);

  pngutil_free_image(&sprites_png);
  cJSON_Delete(meta);
  return 0;
}
