
extern int wavout_enable;
extern unsigned long cur_play;

//サウンドバッファ１バンクあたりの容量。４バンクで適当にラウンドロビン
//PGA_SAMPLESの倍数にすること。PGA_SAMPLESと同じだと多分ダメなので注意。 - LCK
#define MAX_SOUND_BANKLEN 2048

extern short sound_buf[MAX_SOUND_BANKLEN*4*2];

void wavoutClear();
int wavoutInit();

