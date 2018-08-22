//
// Created by Maciej Barczak on 03.08.2018.
//

#include "menu_credits.h"
static unsigned long get_text_color() {
	return setting.color[3];
}

//  "---------------------------------------------------",
static char* credits[] = {
	"     RIN REWIND MOD CREDITS AND VERSION HISTORY",
	"",
	"",
	"Mr. Mirakichi - 2005/08/22",
	"  Original version of RIN 1.32",
	"",
	"DaveX (efengeler@gmail.com) - around 2007",
	"  RIN 1.32 Rewind and Mirror mod",
	"",
	"Papiex (github.com/mbarczak) - summer 2018",
	"  Rewind mod v2 (this version):",
	"    - Sources ported to latest PSP SDK,",
	"    - Dedicated menu for rewind,",
	"    - Support for extra memory (PSP Slim),",
	"    - Adjustable memory and states amount in rewind,",
	"    - Progress bar for actual rewind status,",
	"    - Fixed crash when loading game with a different",
	"      save size than the previous one,",
	"    - Fixed crash when putting PSP to sleep while",
	"      Turbo mode is engaged.",
};
static int credits_len = sizeof(credits)/ sizeof(char*);

void rin_menu_credits() {
	int i;
	for (;;) {
		readpad();
		if (new_pad & (CTRL_CIRCLE|CTRL_TRIANGLE|CTRL_CROSS|CTRL_SQUARE)) {
			return;
		}
		rin_frame("Credits","Å¢ÅõÅ~Å†ÅFReturn to previous menu ");
		unsigned y = 4;
		unsigned x = 4;
		for(i = 0 ; i < credits_len ; ++i){
			pgPrintf(x,y+i,get_text_color(), credits[i]);
		}
		pgScreenFlipV();
	}
}
