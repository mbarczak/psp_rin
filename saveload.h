#ifndef SAVE_H
#define SAVE_H

void save_config();
void load_config();
int save_sram(byte *buf,int size);
int load_sram(byte *buf, int bufsize);
int save_thumb(int slot);
int load_thumb(int slot, unsigned short *out, size_t outlen);
int load_thumb_old(int slot, unsigned short *out, size_t outlen);
int save_state(int slot);
int load_state(int slot);
byte *save_state_tmp();
int load_state_tmp(byte *buf);
int delete_state(int slot);
void get_state_path(int slot, char *out);
void get_thumb_path(int slot, char *out);

#endif
