//
// Created by Maciej Barczak on 02.07.2018.
//

#ifndef RIN_TOOLS_H
#define RIN_TOOLS_H
static inline unsigned byte2mb(unsigned bytes){
	return bytes/1024/1024;
}

static inline unsigned byte2kb(unsigned bytes){
	return bytes/1024;
}

#endif //RIN_TOOLS_H
