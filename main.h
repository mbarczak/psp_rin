#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "syscall.h"
#include "pg.h"
#include "gbcore/gb.h"
#include "sound.h"
#include "filer.h"
#include "menu.h"
#include "saveload.h"

extern char RinPath[], RomPath[], SavePath[], RomName[], CheatPath[];
extern int bMenu;

void set_cpu_clock(int n);

#endif
