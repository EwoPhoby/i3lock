#ifndef _I3LOCK_CONFIG_H_
#define _I3LOCK_CONFIG_H_

#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>

#include "cursors.h"

struct cmdlist {
  int mods;
  xkb_keysym_t ksym;
  char *command;
  struct cmdlist *next;
};

struct config {
  bool dont_fork;
  bool beep;
  char color[7];
  char verifycolor[7];
  char wrongcolor[7];
  char idlecolor[7];
  int curs_choice;
  bool unlock_indicator;
  char *image_path;
  bool tiling;
  bool ignore_empty_password;
  bool show_failed_attempts;
  bool use24hour;
  struct cmdlist *commands;
};

extern int parse_config(char* filename);

#endif /* _I3LOCK_CONFIG_H_ */
