#include "sheet.h"

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

struct Console {
  struct Sheet *sheet;
  int cur_x, cur_y, cur_c;
  struct Timer *timer;
};

void console_task(struct Sheet *sheet, unsigned int memtotal);

void cons_putchar(struct Console *cons, int ch, char move);
void cons_newline(struct Console *cons);
void cons_run_cmd(char *cmdline, struct Console *cons, int *fat, unsigned int memtotal);
void cons_putstr(struct Console *cons, char *s);
void cons_putnstr(struct Console *cons, char *s, int n);
struct Sheet *open_console(struct Shtctl *shtctl, unsigned int memtotal);

#endif // _CONSOLE_H_