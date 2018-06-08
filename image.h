#ifndef IMAGE_H
#define IMAGE_H

int write_png(char *path, const byte *in, unsigned int width, unsigned int height);
int read_png(char *path, unsigned short *out, size_t outlen);
int read_bitmap(char *path, unsigned short *out, size_t outlen);

int PackBits_encode( byte *in, byte *out, int size );
int PackBits_decode( byte *in, byte *out, int size );

#endif

