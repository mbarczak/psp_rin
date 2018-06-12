#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "lib/png.h"

int write_png(char *path, const byte *in, unsigned int width, unsigned int height)
{
    png_struct *pPngStruct = png_create_write_struct( PNG_LIBPNG_VER_STRING,
													  NULL, NULL, NULL );
	if (!pPngStruct){
		return 0;
	}

    png_info *pPngInfo = png_create_info_struct( pPngStruct );
	if (!pPngInfo){
		png_destroy_write_struct( &pPngStruct, NULL );
		return 0;
	}

	png_byte **buf = (png_byte**)malloc(height*sizeof(png_byte*));
	if (!buf){
        png_destroy_write_struct( &pPngStruct, &pPngInfo );
		return 0;
	}

	unsigned int y;
	for(y=0; y<height; y++)
		buf[y] = (byte*)&in[y*width*3];

    if (setjmp( pPngStruct->jmpbuf )){
		free(buf);
        png_destroy_write_struct( &pPngStruct, &pPngInfo );
        return 0;
    }

    FILE *fp = fopen( path, "wb" );
	if (!fp){
		free(buf);
        png_destroy_write_struct( &pPngStruct, &pPngInfo );
		return 0;
	}

    png_init_io( pPngStruct, fp );
    png_set_IHDR( pPngStruct, pPngInfo, width, height, 8, 
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    png_write_info( pPngStruct, pPngInfo );
    png_write_image( pPngStruct, buf );
    png_write_end( pPngStruct, pPngInfo );

    png_destroy_write_struct( &pPngStruct, &pPngInfo );
    fclose(fp);
	free(buf);

    return 1;
}

static int png_check_sig_custom(png_bytep sig, int num)
{
	return ((int)!png_sig_cmp(sig, (png_size_t)0, (png_size_t)num));
}

int read_png(char *path, unsigned short *out, size_t outlen)
{
	FILE *fp = fopen(path,"rb");
	if(!fp)
		return 0;

	const size_t nSigSize = 8;
	byte signature[nSigSize];
	if (sceIoRead(fileno(fp), signature, sizeof(byte)*nSigSize) != nSigSize){
		fclose(fp);
		return 0;
	}

	if (!png_check_sig_custom( signature, nSigSize )){
		fclose(fp);
		return 0;
	}

	png_struct *pPngStruct = png_create_read_struct( PNG_LIBPNG_VER_STRING,
													 NULL, NULL, NULL );
	if(!pPngStruct){
		fclose(fp);
		return 0;
	}

	png_info *pPngInfo = png_create_info_struct(pPngStruct);
	if(!pPngInfo){
		png_destroy_read_struct( &pPngStruct, NULL, NULL );
		fclose(fp);
		return 0;
	}

	if (setjmp( pPngStruct->jmpbuf )){
		png_destroy_read_struct( &pPngStruct, NULL, NULL );
		fclose(fp);
		return 0;
	}

	png_init_io( pPngStruct, fp );
	png_set_sig_bytes( pPngStruct, nSigSize );
	png_read_png( pPngStruct, pPngInfo,
			PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING |
			PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_BGR , NULL);

	png_uint_32 width = pPngInfo->width;
	png_uint_32 height = pPngInfo->height;
	int color_type = pPngInfo->color_type;

	if (outlen != width * height * sizeof(unsigned short)){
		png_destroy_read_struct( &pPngStruct, &pPngInfo, NULL );
		fclose(fp);
		return 0;
	}
	
	png_byte **pRowTable = pPngInfo->row_pointers;
	unsigned int x, y;
	byte r, g, b;
	for (y=0; y<height; y++){
		png_byte *pRow = pRowTable[y];
		for (x=0; x<width; x++){
			switch(color_type){
				case PNG_COLOR_TYPE_GRAY:
					r = g = b = *pRow++;
					break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:
					r = g = b = *pRow++;
					pRow++;
					break;
				case PNG_COLOR_TYPE_RGB:
					b = *pRow++;
					g = *pRow++;
					r = *pRow++;
					break;
				case PNG_COLOR_TYPE_RGB_ALPHA:
					b = *pRow++;
					g = *pRow++;
					r = *pRow++;
					pRow++;
					break;
				default:
					r = g = b = 0;
					break;
			}
			*out++ = RGB(r,g,b);
		}
	}
	
	png_destroy_read_struct( &pPngStruct, &pPngInfo, NULL );
	fclose(fp);

	return 1;
}

int read_bitmap(char *path, unsigned short *out, size_t outlen)
{
	/*
	FILE *fp = fopen(path,"rb");
	if(!fp)
		return 0;
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	byte *buf = (byte*)malloc(size);
	if(!buf){
		fclose(fp);
		return 0;
	}
	fread(buf, 1, size, fp);
	fclose(fp);
	*/
	int fd = sceIoOpen(path, SCE_O_RDONLY, 644);
	if(fd<0)
		return 0;
	size_t size = sceIoLseek(fd, 0, SEEK_END);
	sceIoLseek(fd, 0, SEEK_SET);
	byte *buf = (byte*)malloc(size);
	if(!buf){
		sceIoClose(fd);
		return 0;
	}
	sceIoRead(fd, buf, size);
	sceIoClose(fd);

	if ((size - 0x36)/3 != outlen/2){
		free(buf);
		return 0;
	}
	
	byte *menu_bg = buf + 0x36;
	byte r, g, b;
	unsigned int x, y, yy;
	for(y=0; y<272; y++){
		for(x=0; x<480; x++){
			yy = 271 - y;
			r = *(menu_bg + (yy*480 + x)*3 + 2);
			g = *(menu_bg + (yy*480 + x)*3 + 1);
			b = *(menu_bg + (yy*480 + x)*3);
			*out++=RGB(r,g,b);
		}
	}
	free(buf);
	return 1;
}

/*
// PackBitsˆ³k•„†‰»
int PackBits_encode( byte *in, byte *out, int size )
{
	byte ltr[128], bak, cur, *wp=out;
	int n=1, m=0, i=0, j;
	
	bak = in[i++];
	while(i<size){
		cur = in[i++];
		if(cur != bak){
			if(n>=2){
				*wp++ = -(n-1);
				*wp++ = bak;
			}else{
				ltr[m++] = bak;
				if(m >= 128){
					*wp++ = m-1;
					for(j=0; j<m; j++) *wp++ = ltr[j];
					m=0;
				}
			}
			bak = cur;
			n = 1;
		}else{
			if(m >= 1){
				*wp++ = m-1;
				for(j=0; j<m; j++) *wp++ = ltr[j];
				m = 0;
			}
			if(n < 128){
				n++;
			}else{
				*wp++ = -(n-1);
				*wp++ = bak;
				n = 1;
			}
		}
	}
	if(n>=2){
		if(m >= 1){
			*wp++ = m-1;
			for(j=0; j<m; j++) *wp++ = ltr[j];
		}
		*wp++ = -(n-1);
		*wp++ = bak;
	}else{
		ltr[m++] = bak;
		*wp++ = m-1;
		for(j=0; j<m; j++) *wp++ = ltr[j];
	}
	
	return wp - out;
}
*/

// PackBitsˆ³k•¡‡‰»
int PackBits_decode( byte *in, byte *out, int size )
{
	byte *limit=in+size, *wp=out;
	int i, c;
	
	while( in < limit ){
		c = (char)(*in++);
		if(c < 0){
			for(i=1-c; i>0; i--) *wp++ = *in;
			in++;
		}else{
			for(i=1+c; i>0; i--) *wp++ = *in++;
		}
	}
	
	return wp - out;
}

