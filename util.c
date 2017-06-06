#include <wordexp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include "util.h"

int verify_hex(const char *arg, char *colortype, char *varname) {
  /* Skip # if present */
  int offset = 0;
  if (arg[0] == '#') {
    offset++;
  }

  if (strlen(arg+offset) != 6 ||
      sscanf(arg+offset, "%06[0-9a-fA-F]", colortype) != 1) {
    errx(EXIT_FAILURE, "%s is invalid, it must be given in 3-byte hexadecimal format: rrggbb\n", varname);
    return 1;
  }
  return 0;
}

char* expand_path(char* path) {
  wordexp_t we;
  wordexp(path, &we, 0);
  if (we.we_wordc < 1) {
    return path;
  }
  char *heapcopy = strdup(we.we_wordv[0]);
  wordfree(&we);
  return heapcopy;
}

char* trim(const char* str) {
  if (str == NULL) {
    return NULL;
  }
  int len = strlen(str);
  int left = 0, right = len-1;
  /* Find left trimming offset */
  for (; left < len; ++left) {
    if (str[left] != ' ' && str[left] != '\n' && str[left] != '\r' &&
        str[left] != '\t' && str[left] != '\f') {
      break;
    }
  }
  for (; right > left; --right) {
    if (str[right] != ' ' && str[right] != '\n' && str[right] != '\r' &&
        str[right] != '\t' && str[right] != '\f') {
      break;
    }
  }
  return strndup(str+left, right-left+1);
}

