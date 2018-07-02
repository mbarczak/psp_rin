//
// Created by Maciej Barczak on 02.07.2018.
//

#ifndef RIN_TOOLS_H
#define RIN_TOOLS_H
static inline long byte2mb(long bytes){
	return bytes/1024/1024;
}

static inline long byte2kb(long bytes){
	return bytes/1024;
}

#endif //RIN_TOOLS_H
