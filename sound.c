#include "main.h"

unsigned long cur_play=0;
int wavout_enable=0;


short sound_buf[MAX_SOUND_BANKLEN*4*2];


//44100,chan:2ŒÅ’è
static void wavout_snd0_callback(short *buf, unsigned long reqn)
{
	if (!wavout_enable) {
		memset(buf,0,reqn*4);
		return;
	}

	int sound_buf_len = 768 + 256 * setting.sound_buffer;
	unsigned int ptr=cur_play;
	unsigned int nextptr=ptr+reqn;
	if (nextptr>=sound_buf_len*4) nextptr=0;
	__memcpy4a((unsigned long *)buf, (unsigned long *)&sound_buf[ptr*2], reqn);
//	__memcpy4((unsigned long *)buf, (unsigned long *)&sound_buf[ptr*2], reqn);
	cur_play=nextptr;
}

void wavoutClear()
{
	memset(sound_buf, 0, sizeof(sound_buf));
}

//return 0 if success
int wavoutInit()
{
	wavout_enable=0;
	pgaSetChannelCallback(0,wavout_snd0_callback);
	return 0;
}

