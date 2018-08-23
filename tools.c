//
// Created by Maciej Barczak on 19.08.2018.
//
#include <stdio.h>
#include <memory.h>
#include "tools.h"

char *get_progress_bar(int max, int act) {
#define BAR_LENGTH  60
#define MAX_PROGRESS (BAR_LENGTH+1)
#define MAX_STR 200
	const char LIMITER_CHAR_LEFT = 11;
	const char LIMITER_CHAR_RIGHT = 12;
	const char ACT_CHAR = 9;
	const char BUILD_CHAR = 10;
	static char progress[MAX_PROGRESS];
//	static char string[MAX_STR];
	if (max == 0) {
		progress[0] = '\0';
	} else {
		memset(progress, BUILD_CHAR, MAX_PROGRESS);

		int actPercentage = MAX(MIN((100 * act) / max, 100), 0);
		int actInBar = (((BAR_LENGTH - 2) * actPercentage) / 100) + 1;

		progress[0] = LIMITER_CHAR_LEFT;
		progress[BAR_LENGTH - 1] = LIMITER_CHAR_RIGHT;
		progress[BAR_LENGTH] = '\0';
		if (actInBar == 0) {
			actInBar = 1;
		} else if (actInBar == BAR_LENGTH - 1) {
			actInBar = BAR_LENGTH - 2;
		}
		progress[actInBar] = ACT_CHAR;

//		memset(string, 0, MAX_STR);
//		snprintf(string, MAX_STR, " Rewind :%s", progress);
//		snprintf(string, MAX_STR, "%s", progress);
	}
	return progress;
}

//#define TESTING

#ifdef TESTING

#include <stdio.h>

int main() {
	char *progress = get_progress_bar( 1478, 1500);
	printf("Progress : %s , length: %d\n", progress, strlen(progress));

	return 0;
}

#endif // TESTING
