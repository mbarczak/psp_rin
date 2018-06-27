/*
Author: davex
e-mail: efengeler@gmail.com
*/

#include "rewind.h"



int num_rwnd_states = 0;
int rwnd_state_size = 0;
const int TOTAL_REWIND_MEMORY = 5*1024*1024; //reserves 5 MB for rewind states
int g_rwnd_period = 0;

struct rewind_state{
	int have_data;
	unsigned char *data;
	struct rewind_state *next;
	struct rewind_state *prev;
};


struct rewind_state *ptr_rewind_states, *prev_state, *next_state;


void allocate_rewind_states(void){
	struct rewind_state *created_state, *first_state;
	int i;
	rwnd_state_size = gb_save_state(NULL);  //declared in "gbcore/gb.h"
	num_rwnd_states = (int) ( (float)TOTAL_REWIND_MEMORY / (float) rwnd_state_size );
	
	//reserves first state
	created_state =  (struct rewind_state *)malloc( sizeof(struct rewind_state) );
	created_state->have_data = 0;
	created_state->data = (byte *) malloc( rwnd_state_size);
	first_state = created_state;
	prev_state = first_state;
	
	//reserves remaining states
	for( i = 1; i< num_rwnd_states; i++){
		created_state  = (struct rewind_state *)malloc( sizeof(struct rewind_state) );
		created_state->have_data = 0;
		created_state->data = (byte *) malloc( rwnd_state_size);
		created_state ->prev = prev_state;
		prev_state->next = created_state;
		prev_state = created_state;
	}
	
	
	//make list be circular
	created_state->next = first_state; 
	first_state->prev = created_state; 
	ptr_rewind_states = first_state;
	
}


void free_rewind_states(void){
	
	struct rewind_state *now_state; 
	
	now_state = ptr_rewind_states;
	if(ptr_rewind_states){
        prev_state = now_state->prev;
        prev_state->next = NULL;

        while(1){
            if ( now_state == NULL)
                break;
            next_state = now_state->next;
            free(now_state->data );
            free(now_state);
            now_state = next_state;
        }
	}
}


void save_rewind_state(void){
	
	gb_save_state(ptr_rewind_states->data); //declared in "gbcore/gb.h"
	ptr_rewind_states->have_data = 1;
	ptr_rewind_states = ptr_rewind_states->next;
}

int read_rewind_state(void){
	
	int ret_val = -999;
	prev_state = ptr_rewind_states->prev;

	if (prev_state->have_data > 0 ){
		load_state_tmp( prev_state->data); //declarado en "saveload.h"
		prev_state->have_data = 0;
		ptr_rewind_states = ptr_rewind_states->prev;
		ret_val = 1;
	}
	return ret_val;
}

