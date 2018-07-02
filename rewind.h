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

#define MAX_MEMORY_ESTIMATION

void allocate_rewind_states(void);
int read_rewind_state(void);
void save_rewind_state(void);
void free_rewind_states(void);

#ifdef MAX_MEMORY_ESTIMATION
void test_available_memory(void);
#endif
int establish_max_rewind_memory(void);
extern int max_rewind_memory;

#endif
