/*
Author: davex
e-mail: efengeler@gmail.com
*/

#ifndef REWIND_H
#define REWIND_H


#include <stdlib.h>
#include "gbcore/gb.h"
#include "saveload.h"
#include "main.h"

//#define MAX_MEMORY_ESTIMATION

#define REWIND_MIN_USER_MEMORY 0
#define REWIND_MAX_USER_MEMORY (100 * 1024 * 1024)
#define REWIND_MIN_USER_STATES 0
#define REWIND_MAX_USER_STATES 500

enum{
    REWIND_MODE_LIMIT_MEMORY_AMOUNT = 0,
    REWIND_MODE_LIMIT_STATES_AMOUNT = 1
};

void allocate_rewind_states(void);
int read_rewind_state(void);
void save_rewind_state(void);
void free_rewind_states(void);

extern unsigned int max_rewind_memory;
extern int num_rwnd_states;

#ifdef MAX_MEMORY_ESTIMATION
void test_available_memory(void);
#endif
unsigned establish_max_rewind_memory(void);
unsigned get_current_rewind_memory(SETTING local);
void print_rewind_debug_info(SETTING local,char* info);

#endif
