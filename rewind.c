/*
Author: davex
e-mail: efengeler@gmail.com
*/

#include "rewind.h"
#include "tools.h"
#include "menu.h"


int num_rwnd_states = 0;
int rwnd_state_size = 0;
const int TOTAL_REWIND_MEMORY = 5*1024*1024; //reserves 5 MB for rewind states
const int SAFE_MEMORY_MARGIN = 1*1024*1024;//left for system to use
unsigned max_rewind_memory = 5*1024*1024;
int g_rwnd_period = 0;

struct rewind_state{
	int have_data;
	unsigned char *data;
	struct rewind_state *next;
	struct rewind_state *prev;
};


struct rewind_state *ptr_rewind_states, *prev_state, *next_state;


static void set_number_of_rewind_states(int *number_of_states);

void allocate_rewind_states(void){
	if(setting.rewind_enabled){
		struct rewind_state *created_state, *first_state;
		int i;
		rwnd_state_size = gb_save_state(NULL);  //declared in "gbcore/gb.h"
		set_number_of_rewind_states(&num_rwnd_states);
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

	print_rewind_debug_info(setting,"allocate_rewind_states");
}

static void set_number_of_rewind_states(int *number_of_states) {
	switch (setting.rewind_limit_mode){
		case REWIND_MODE_LIMIT_MEMORY_AMOUNT:
			(*number_of_states) = (int) ( (float)get_current_rewind_memory(setting) / (float) rwnd_state_size );
			break;
		case REWIND_MODE_LIMIT_STATES_AMOUNT:{
			int max_states = (int) ( (float)max_rewind_memory / (float) rwnd_state_size );
			if(setting.rewind_always_use_max_states){
				(*number_of_states) = max_states;
			}else{
				(*number_of_states) = MIN(max_states,setting.rewind_user_max_states_ammount);
			}
		}
			break;
		default:
			break;
	}
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
        ptr_rewind_states = NULL;
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

//Papiex : Rewind extension

static unsigned checkAvailableMemoryBiggestSingleChunk(){
	void* mem = NULL;
    unsigned size = 1024;
    unsigned incr = 1024;
	while((mem = malloc(size))){
		free(mem);
		size += incr;
	}
	return size-incr;
}

typedef struct head{
	void * mem;
	struct head* next;
} list;

list* list_alloc(){
	return calloc(1,sizeof(list));
}

void list_destroy(list** head){
	if(*head){
		list* iter = *head;
		while(iter){
			list* next = iter->next;
			free(iter->mem);
			free(iter);
			iter = next;
		}
		(*head) = NULL;
	}
}

list* list_add(list** head, void* mem){
	if(head && (*head)){
		list* elem = list_alloc();
		if(elem){
			elem->mem = mem;
			elem->next = (*head);
			(*head) = elem;
		}
		return (*head);
	}
	return NULL;
}

static unsigned checkAvailableMemoryMixedChunks(){
	unsigned retval = 0;
	void* mem = NULL;
	list* head = list_alloc();
	size_t size = 1024*512;
	int len = 0;

	while((mem = malloc(size))){
		if(!list_add(&head,mem)){
			free(mem);
			break;
		}
		else {
			len++;
		}
	}
	retval = len * size;
	list_destroy(&head);
	return retval;
}
#ifdef MAX_MEMORY_ESTIMATION

static int checkIfButtonRepressed(){
	int retVal = 0;
	ctrl_data_t paddata;
	sceCtrlPeekBufferPositive(&paddata, 1);
	if((paddata.buttons & PSP_CTRL_LEFT)
	   && (paddata.buttons & PSP_CTRL_START))
	{
		retVal = 1;
	}
	return retVal;
}

void test_available_memory(void){
	static int testNumber=0;
#define MSG_LEN 255
	static char tmp[MSG_LEN]={0};
	ctrl_data_t paddata;
	sceCtrlPeekBufferPositive(&paddata, 1);
	if(checkIfButtonRepressed()){
		unsigned availBiggest = checkAvailableMemoryBiggestSingleChunk();
        unsigned availMixed = checkAvailableMemoryMixedChunks();
		snprintf(tmp,MSG_LEN,"Mem:%d,big chunk: %d mb (%d kb);small:%d mb (%d kb)\n",
				 testNumber,byte2mb(availBiggest),byte2kb(availBiggest),byte2mb(availMixed),byte2kb(availMixed));
		printf(tmp);
		testNumber++;
		pgWaitVn(20);

	}
	pgPrintf(0,1,RGB(255,0,0),tmp);
}
#endif //MAX_MEMORY_ESTIMATION
unsigned establish_max_rewind_memory(void) {
    unsigned retval = checkAvailableMemoryBiggestSingleChunk() - SAFE_MEMORY_MARGIN;
    return retval;
}


unsigned get_current_rewind_memory(SETTING local) {
	unsigned retval;
	if(local.rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT){
		if(local.rewind_always_use_max_memory){
			retval = max_rewind_memory;
		}else{
			retval = MIN(local.rewind_user_max_memory_ammount,max_rewind_memory);
		}
	}else{//just informative estimation for displaying in menu
		retval = num_rwnd_states * (sizeof(struct rewind_state)+rwnd_state_size);
	}
    return retval;
}

char* get_current_rewind_memory_string(SETTING local){
	static char* retval[10];
	ftoa(byte2mb_asFloat(get_current_rewind_memory(local)),(char*)retval,1);
	return (char*)retval;
}

void print_rewind_debug_info(SETTING local, char *info) {
	printf("\nINFO TYPE : %s\n",info);
	printf("\tGlobal current rewind memory    - get_current_rewind_memory()     : %d bytes / %d kb / %s mb\n",
	       get_current_rewind_memory(local),byte2kb(get_current_rewind_memory(local)),get_current_rewind_memory_string(local));
	printf("\tGlobal number of states         - num_rwnd_states                 : %d\n",num_rwnd_states);
	printf("\tGlobal state size               - rwnd_state_size                 : %d bytes / %d kb\n",rwnd_state_size,byte2kb(rwnd_state_size));
	char floatStringMaxRewindMemory[10] = {0};
	ftoa(byte2mb_asFloat(max_rewind_memory),floatStringMaxRewindMemory,1);
	printf("\tGlobal max avail. rewind memory - max_rewind_memory               : %d bytes / %d kb / %s mb\n",
	       max_rewind_memory,byte2kb(max_rewind_memory),floatStringMaxRewindMemory);
	printf("\tRewind : enabled                - rewind_enabled                  : %d\n",local.rewind_enabled);
	printf("\tRewind : limit mode             - rewind_limit_mode               : %s\n",local.rewind_limit_mode?"REWIND_MODE_LIMIT_STATES_AMOUNT":"REWIND_MODE_LIMIT_MEMORY_AMOUNT");
	printf("\tRewind : always use max states  - rewind_always_use_max_states    : %d\n",local.rewind_always_use_max_states);
	printf("\tRewind : user max states amount - rewind_user_max_states_ammount  : %d\n",local.rewind_user_max_states_ammount);
	printf("\tRewind : always use max memory  - rewind_always_use_max_memory    : %d\n",local.rewind_always_use_max_memory);
	char floatStringUserMaxRewindMemory[10] = {0};
	ftoa(byte2mb_asFloat(local.rewind_user_max_memory_ammount),floatStringUserMaxRewindMemory,1);
	printf("\tRewind : user max memory amount - rewind_user_max_memory_ammount  : %d bytes / %d kb / %s mb\n\n",
	       local.rewind_user_max_memory_ammount,byte2kb(local.rewind_user_max_memory_ammount),floatStringUserMaxRewindMemory);


}
