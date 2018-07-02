#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <psptypes.h>
#include <psprtc.h>
#include <pspiofilemgr.h>
#include <pspthreadman.h>
#include <pspctrl.h>
#include <psppower.h>
#include <psploadexec.h>
#include <pspuser.h>
#include <pspaudio.h>
#include <pspdisplay.h>
#include <pspge.h>

/* sceSuspendForUser */
// available in sdk/lib/libpspuser.a , but not available in any header. TODO: check if can be called directly.
int sceKernelPowerLock(int locktype);
int sceKernelPowerUnlock(int locktype);


/* IoFileMgrForUser */

#define SCE_O_RDONLY    PSP_O_RDONLY
#define SCE_O_WRONLY    PSP_O_WRONLY
#define SCE_O_RDWR      PSP_O_RDWR
#define SCE_O_NBLOCK    PSP_O_NBLOCK
#define SCE_O_APPEND    PSP_O_APPEND
#define SCE_O_CREAT     PSP_O_CREAT
#define SCE_O_TRUNC     PSP_O_TRUNC
#define SCE_O_EXCL      PSP_O_EXCL
#define SCE_O_NOWAIT    PSP_O_NOWAIT

/******************************************************************************/
/* sceCtrl */
/* Index for the two analog directions */
#define CTRL_ANALOG_X   0
#define CTRL_ANALOG_Y   1

/* Button bit masks */
#define CTRL_SQUARE		0x008000
#define CTRL_TRIANGLE	0x001000
#define CTRL_CIRCLE		0x002000
#define CTRL_CROSS		0x004000
#define CTRL_UP			0x000010
#define CTRL_DOWN		0x000040
#define CTRL_LEFT		0x000080
#define CTRL_RIGHT		0x000020
#define CTRL_START		0x000008
#define CTRL_SELECT		0x000001
#define CTRL_LTRIGGER	0x000100
#define CTRL_RTRIGGER	0x000200
#define CTRL_HOLD		0x020000

/* Returned control data */
typedef struct _ctrl_data
{
    u32 frame;
    u32 buttons;
    u8  analog[4];
    u32 unused;
} ctrl_data_t;

#define MAX_PATH 512
#define MAX_NAME 256
#define MAX_ENTRY 2048


//#include "syscall.h"
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
