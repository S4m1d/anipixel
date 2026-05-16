#include "txtutil.h"
#include <stdio.h>
#include <stdlib.h>

char *txtutil_read(char *path) {
  FILE *fp = fopen(path, "rb");
  if (!fp) {
    fprintf(stderr, "txtutil_read: failed to open file %s\n", path);
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *buf = malloc(size + 1);
  fread(buf, 1, size, fp);
  buf[size] = '\0';
  fclose(fp);

  return buf;
}
