//
// Created by Maciej Barczak on 02.07.2018.
//

#ifndef RIN_TOOLS_H
#define RIN_TOOLS_H

#define MAX(a,b) (a)>=(b)?(a):(b)
#define MIN(a,b) (a)<(b)?(a):(b)

static inline unsigned byte2mb(unsigned bytes){
	return bytes/1024/1024;
}

static inline float byte2mb_asFloat(unsigned bytes){
	return bytes/1024.0f/1024.0f;
}

static inline unsigned byte2kb(unsigned bytes){
	return bytes/1024;
}

char* get_progress_bar(int max, int act);
#endif //RIN_TOOLS_H
