#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <wordexp.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>
#include <time.h>

#include "cursors.h"
#include "i3lock_config.h"
#include "util.h"

/* Length of the key -> setter function array */
#define VALID_KEYS_LEN 16

const int CMD_KEY_SHIFT = 1;
const int CMD_KEY_CTRL = 2;
const int CMD_KEY_ALT = 4;
const int CMD_KEY_SUPER = 8;

struct config configuration = {
  .dont_fork = false,
  .beep = false,
  .color = "ffffff",
  .verifycolor = "00ff00",
  .wrongcolor = "ff0000",
  .idlecolor = "000000",
  .circle_alpha = 20,
  .curs_choice = CURS_NONE,
  .unlock_indicator = true,
  .image_path = NULL,
  .tiling = false,
  .ignore_empty_password = false,
  .show_failed_attempts = false,
  .tfstring = "%l:%M %p",
  .dfstring = "",
  .commands = NULL
};

/*
 *  Setters for the single config values
 */

static int set_dont_fork(char* val) {
  if (!strcmp("true", val)) {
    configuration.dont_fork = true;
  } else if (!strcmp("false", val)) {
    configuration.dont_fork = false;
  } else {
    return EINVAL;
  }
  return 0;
}

static int set_beep(char* val) {
  if (!strcmp("true", val)) {
    configuration.beep = true;
  } else if (!strcmp("false", val)) {
    configuration.beep = false;
  } else {
    return EINVAL;
  }
  return 0;
}

static int set_color(char* val) {
  return verify_hex(val, configuration.color, "color");
}

static int set_verify_color(char* val) {
  return verify_hex(val, configuration.verifycolor, "verifycolor");
}

static int set_wrong_color(char* val) {
  return verify_hex(val, configuration.wrongcolor, "wrongcolor");
}

static int set_idle_color(char* val) {
  return verify_hex(val, configuration.idlecolor, "idlecolor");
}

int set_circle_alpha(char* val) {
  errno = 0;
  long la = strtol(val, NULL, 10);
  if (errno) {
    return 1;
  }
  if (la < 0 || la > 100) {
    return 1;
  }
  configuration.circle_alpha = (int) la;
  return 0;
}

static int set_pointer(char* val) {
  if (!strcmp("none", val)) {
    configuration.curs_choice = CURS_NONE;
  } else if (!strcmp("default", val)) {
    configuration.curs_choice = CURS_DEFAULT;
  } else if (!strcmp("win", val)) {
    configuration.curs_choice = CURS_WIN;
  } else {
    return EINVAL;
  }
  return 0;
}

static int set_image(char* val) {
  configuration.image_path = strdup(val);
  return 0;
}

static int set_image_tiling(char* val) {
  if (!strcmp("true", val)) {
    configuration.tiling = true;
  } else if (!strcmp("false", val)) {
    configuration.tiling = false;
  } else {
    return EINVAL;
  }
  return 0;
}

static int set_ignore_empty_password(char* val) {
  if (!strcmp("true", val)) {
    configuration.ignore_empty_password = true;
  } else if (!strcmp("false", val)) {
    configuration.ignore_empty_password = false;
  } else {
    return EINVAL;
  }
  return 0;
}

static int set_show_unlock_indicator(char* val) {
  if (!strcmp("true", val)) {
    configuration.unlock_indicator = true;
  } else if (!strcmp("false", val)) {
    configuration.unlock_indicator = false;
  } else {
    return EINVAL;
  }
  return 0;
}

static int set_show_failed_attempts(char* val) {
  if (!strcmp("true", val)) {
    configuration.show_failed_attempts = true;
  } else if (!strcmp("false", val)) {
    configuration.show_failed_attempts = false;
  } else {
    return EINVAL;
  }
  return 0;
}

int set_fstring(char* val, int is_date) {
  size_t testlen;
  char testout[100];
  time_t rawtime;
  struct tm *tm;
  if (strlen(val) == 0) {
    return 0;
  }
  time(&rawtime);
  tm = localtime(&rawtime);
  testlen = strftime(testout, sizeof(testout), val, tm);
  /* May yield some false negatives, but still better than passing buffers with
     undefined content to Xlib... */
  if (testlen == 0UL) {
    return 1;
  }
  if (is_date) {
    strncpy(configuration.dfstring, val, sizeof(configuration.dfstring));
  } else {
    strncpy(configuration.tfstring, val, sizeof(configuration.tfstring));
  }
  return 0;
}

static int set_tfstring(char *val) {
  return set_fstring(val, 0);
}

static int set_dfstring(char *val) {
  return set_fstring(val, 1);
}

static int append_command(char* val) {
  char *keys;
  char *key;
  char *cmd;
  int mod = 0;
  xkb_keysym_t ksym;
  /* Separate key combination and command to execute */
  char* keys_untr = strtok(val, "=");
  keys = trim(keys_untr);
  char* cmd_untr = strtok(NULL, "");
  cmd = trim(cmd_untr);
  /* Decompose key combination into single keys */
  key = strtok(keys, "+");
  while (key) {
    /* If the key is a modifier, set the appropriate flag */
    if (!strcmp("ctrl", key)) {
      mod |= CMD_KEY_CTRL;
    } else if (!strcmp("alt", key)) {
      mod |= CMD_KEY_ALT;
    } else if (!strcmp("super", key)) {
      mod |= CMD_KEY_SUPER;
    } else if (!strcmp("shift", key)) {
      mod |= CMD_KEY_SHIFT;
    } else {
      /* Convert the key name into a XKB keysym and check whether it actually exists */
      ksym = xkb_keysym_from_name(key, 0);
      if (ksym == XKB_KEY_NoSymbol) {
        fprintf(stderr, "Unknown value: `%s`\n", key);
        return 1;
      }
      /* Prepend a new entry to the configuration's command list */
      struct cmdlist *item = (struct cmdlist*) malloc(sizeof(struct cmdlist));
      item->mods = mod;
      item->ksym = ksym;
      item->command = (char*) malloc((strlen(cmd)+1)*sizeof(char));
      strcpy(item->command, cmd);
      item->next = configuration.commands;
      configuration.commands = item;
      /* Try to get one more key for error checking */
      key = strtok(NULL, "+");
      break;
    }
    key = strtok(NULL, "+");
  }
  /*
   * If a non-modifier key was encountered and there are still other keys,
   * assume an error in the config.
   */
  if (key) {
    fprintf(stderr, "Invalid key combination token: `%s`\n", key);
    return 1;
  }
  return 0;
}

/*
 *  Array of valid config file keys with a pointer to the corresponding
 *  setter function.
 */
struct {
  const char *key;
  int (*setter) (char*);
} valid_keys[VALID_KEYS_LEN] = {
  { .key = "nofork", .setter = set_dont_fork },
  { .key = "beep", .setter = set_beep },
  { .key = "color", .setter = set_color },
  { .key = "verify-color", .setter = set_verify_color },
  { .key = "wrong-color", .setter = set_wrong_color },
  { .key = "idle-color", .setter = set_idle_color },
  { .key = "circle-alpha", .setter = set_circle_alpha },
  { .key = "pointer", .setter = set_pointer },
  { .key = "image", .setter = set_image },
  { .key = "tiling", .setter = set_image_tiling },
  { .key = "ignore-empty-password", .setter = set_ignore_empty_password },
  { .key = "unlock-indicator", .setter = set_show_unlock_indicator },
  { .key = "show-failed-attempts", .setter = set_show_failed_attempts },
  { .key = "time-format", .setter = set_tfstring },
  { .key = "date-format", .setter = set_dfstring },
  { .key = "command", .setter = append_command }
};


int parse_config(char* filename) {
  /* Expand file name (e.g. `~` -> `/home/username`) and open file */
  char *fexp = expand_path(filename);
  errno = 0;
  FILE *conffile = fopen(fexp, "r");
  if (!conffile) {
    return errno;
  }
  /* Read line after line */
  char *linebuf = (char*) malloc(sizeof(char)*4096);
  for (int lc = 1; fgets(linebuf, 4096, conffile); ++lc) {
    /* Extract option key and value */
    char *untr_key = strdup(strtok(linebuf, "="));
    char *key = trim(untr_key);
    if (key == NULL || strlen(key) == 0 || key[0] == '#') {
      /* Skip, if the line is empty or a comment (starts with #) */
      continue;
    }
    char *untr_val = strdup(strtok(NULL, ""));
    char *val = trim(untr_val);
    free(untr_key);
    free(untr_val);
    /* 
     * Search a matching key in the list of valid keys and call the
     * corresponding setter. If an invalid key or value is found, print
     * an error, but ignore it, as we want to lock the screen after all.
     */
    bool valid_key = false;
    for (int i = 0; i < VALID_KEYS_LEN; ++i) {
      if (!strcmp(valid_keys[i].key, key)) {
        valid_key = true;
        if (valid_keys[i].setter(val)) {
          errx(EXIT_FAILURE, "Parsing error: `%s` (%s:%d)", val, filename, lc);
        }
      }
    }
    if (!valid_key) {
      errx(EXIT_FAILURE, "Unknown key: `%s` (%s:%d)", key, filename, lc);
    }
    free(key);
    free(val);
  }
  
  fclose(conffile);
  free(linebuf);
  return 0;
}
