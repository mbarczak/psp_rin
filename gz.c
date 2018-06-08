#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gz.h"

int gzGetOrigName(const char *path, char *out, gzFile fd)
{
	if (!fd)
		return 0;

	if (((gz_stream*)fd)->transparent == 1)
		return 0;

	if (((gz_stream*)fd)->orig_name[0] == 0){
		strcpy(out, strrchr(path,'/')+1);
		char *p = strrchr(out, '.');
		if (p && !stricmp(p, ".gz")){
			*p = 0;
			return 1;
		}
		return 0;
	}

	strcpy(out, ((gz_stream*)fd)->orig_name);

	return 1;
}

int gzCreate(int fd, unsigned char *inbuf, u32 inlen)
{
	const u8 header[] = {0x1f,0x8b,Z_DEFLATED,0,0,0,0,0,0,OS_CODE};

	u32 outlen = ((inlen+12)/100+1)*101;
	u8 *outbuf = (u8*)malloc(outlen+sizeof(header)+sizeof(u32)*2);
	if (!outbuf) return 0;

	z_stream s;
    s.zalloc = (alloc_func)0;
    s.zfree = (free_func)0;
    s.opaque = (voidpf)0;
    s.next_in = Z_NULL;
    s.next_out = Z_NULL;
    s.avail_in = s.avail_out = 0;

	int err;
	err = deflateInit2(&s, Z_DEFAULT_COMPRESSION,
		Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
	if (err!=Z_OK){
		free(outbuf);
		return 0;
	}

	s.avail_in = inlen;
	s.avail_out = outlen;
	s.next_in = inbuf;
	s.next_out = outbuf+sizeof(header);

	err = deflate(&s, Z_FINISH);
	if (err != Z_STREAM_END){
		err = deflateEnd(&s);
		free(outbuf);
		return 0;
	}
	err = deflateEnd(&s);

	u32 size = outlen - s.avail_out;
	u32 crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, inbuf, inlen);

	memcpy(outbuf, header, sizeof(header));
	memcpy(outbuf+sizeof(header)+size, &crc, sizeof(u32));
	memcpy(outbuf+sizeof(header)+size+sizeof(u32), &inlen, sizeof(u32));

	int writesize = sceIoWrite(fd, outbuf, size+sizeof(header)+sizeof(u32)*2);
	free(outbuf);

	if (writesize != size+sizeof(header)+sizeof(u32)*2)
		return 0;
	else
		return 1;
}

