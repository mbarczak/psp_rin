// primitive graphics for Hello World sce

#include <stdio.h>
#include <stdarg.h>

//#include "syscall.h"
#include "main.h"
#include "pg.h"
#include "gbcore/gb.h"

#include "font.c"
#include "fontNaga10.c"

static unsigned int *GEcmd = (unsigned int *)0x441CC000;
static short *ScreenVertex = (short *)0x441CC100;
static int gecbid = -1;

//variables
#define pg_vramtop ((char *)0x04000000)
long pg_screenmode;
long pg_showframe;
long pg_drawframe;
unsigned long pgc_csr_x[2], pgc_csr_y[2];
unsigned long pgc_fgcolor[2], pgc_bgcolor[2];
char pgc_fgdraw[2], pgc_bgdraw[2];
char pgc_mag[2];

void pgWaitVn(unsigned long count)
{
	for (; count>0; --count) {
		sceDisplayWaitVblankStart();
	}
}


void pgWaitV()
{
	sceDisplayWaitVblankStart();
}


char *pgGetVramAddr(unsigned long x,unsigned long y)
{
	return pg_vramtop+(pg_drawframe?FRAMESIZE:0)+x*PIXELSIZE*2+y*LINESIZE*2+0x40000000;
//	return pg_vramtop+(pg_drawframe?FRAMESIZE:0)+x*PIXELSIZE*2+y*LINESIZE*2;//+0x40000000;	//変わらないらしい
}

void pgPrint_drawbg(unsigned long x, unsigned long y, unsigned long color, unsigned long bgcolor, const char *str)
{
	while (*str!=0 && x<CMAX_X && y<CMAX_Y) {
		pgPutChar(x*8,y*8,color,bgcolor,*str,1,1,1);
		str++;
		x++;
		if (x>=CMAX_X) {
			x=0;
			y++;
		}
	}
}

void pgPrintf(unsigned long x,unsigned long y,unsigned long color,const char *str, ...)
{
	va_list ap;
	char szBuf[512];

	va_start(ap, str);
	vsprintf(szBuf, str, ap);
	va_end(ap);

	pgPrint(x,y,color,szBuf);
}

void pgPrint(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX_X && y<CMAX_Y) {
		pgPutChar(x*8,y*8,color,0,*str,1,0,1);
		str++;
		x++;
		if (x>=CMAX_X) {
			x=0;
			y++;
		}
	}
}

void pgPrint2(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX2_X && y<CMAX2_Y) {
		pgPutChar(x*16,y*16,color,0,*str,1,0,2);
		str++;
		x++;
		if (x>=CMAX2_X) {
			x=0;
			y++;
		}
	}
}


void pgPrint4(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX4_X && y<CMAX4_Y) {
		pgPutChar(x*32,y*32,color,0,*str,1,0,4);
		str++;
		x++;
		if (x>=CMAX4_X) {
			x=0;
			y++;
		}
	}
}

void pgDrawFrame(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, unsigned long color)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned long i;

	vptr0=(unsigned char *)pgGetVramAddr(0,0);
	for(i=x1; i<=x2; i++){
		((unsigned short *)vptr0)[i*PIXELSIZE + y1*LINESIZE] = color;
		((unsigned short *)vptr0)[i*PIXELSIZE + y2*LINESIZE] = color;
	}
	for(i=y1; i<=y2; i++){
		((unsigned short *)vptr0)[x1*PIXELSIZE + i*LINESIZE] = color;
		((unsigned short *)vptr0)[x2*PIXELSIZE + i*LINESIZE] = color;
	}
}

void pgFillBox(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, unsigned long color)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned long i, j, tmp;

	if (x1 > x2){
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2){
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	vptr0=(unsigned char *)pgGetVramAddr(0,0);
	for(i=y1; i<=y2; i++){
		for(j=x1; j<=x2; j++){
			((unsigned short *)vptr0)[j*PIXELSIZE + i*LINESIZE] = color;
		}
	}
}

void pgFillvram(unsigned long color)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned long i;

	vptr0=(unsigned char *)pgGetVramAddr(0,0);
	for (i=0; i<FRAMESIZE/2; i++) {
		*(unsigned short *)vptr0=color;
		vptr0+=PIXELSIZE*2;
	}
}

void pgBitBlt(unsigned long x,unsigned long y,unsigned long w,unsigned long h,unsigned long mag,const unsigned short *d)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	unsigned long xx,yy,mx,my;
	const unsigned short *dd;

	vptr0=(unsigned char *)pgGetVramAddr(x,y);
	for (yy=0; yy<h; yy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			dd=d;
			for (xx=0; xx<w; xx++) {
				for (mx=0; mx<mag; mx++) {
					*(unsigned short *)vptr=*dd;
					vptr+=PIXELSIZE*2;
				}
				dd++;
			}
			vptr0+=LINESIZE*2;
		}
		d+=w;
	}

}

void pgBitBltSgb(unsigned long x,unsigned long y,unsigned long *d)
{
	unsigned long *v0;		//pointer to vram
	unsigned long yy;

	v0=(unsigned long *)pgGetVramAddr(x,y);
	for (yy=0; yy<224; yy++) {
		__memcpy4a(v0,d,256/2);
		v0+=(LINESIZE/2-256/2);
	}
}

//ちょい早いx1 - LCK
void pgBitBltN1(unsigned long x,unsigned long y,unsigned long *d)
{
	unsigned long *v0;		//pointer to vram
	unsigned long yy;

	v0=(unsigned long *)pgGetVramAddr(x,y);
	d+=GUARD_LINE/2;
	for (yy=0; yy<144; yy++) {
		__memcpy4a(v0,d,80);
		v0+=(LINESIZE/2-80);
		d+=GUARD_LINE;
	}
}

//あんまり変わらないx1.5 -LCK
void pgBitBltN15(unsigned long x,unsigned long y,unsigned long *d)
{
	unsigned short *vptr0;		//pointer to vram
	unsigned short *vptr;		//pointer to vram
	unsigned long xx,yy;
	
	vptr0=(unsigned short *)pgGetVramAddr(x,y);
	d+=GUARD_LINE/2;
	for (yy=0; yy<72; yy++) {
		unsigned long *d0=d+(yy*2)*SIZE_LINE/2;
		vptr=vptr0;
		for (xx=0; xx<80; xx++) {
			unsigned long dd1,dd2,dd3,dd4;
			unsigned long dw;
			dw=d0[0];
			dd1=((vptr[0]           =((dw)     & 0x739c))) ;
			dd2=((vptr[2]           =((dw>>16) & 0x739c))) ;
			dw=d0[SIZE_LINE/2];
			dd3=((vptr[0+LINESIZE*2]=((dw)     & 0x739c))) ;
			dd4=((vptr[2+LINESIZE*2]=((dw>>16) & 0x739c))) ;

			vptr++;
			*vptr=(dd1+dd2) >> 1;
			vptr+=(LINESIZE-1);
			*vptr=(dd1+dd3) >> 1;
			vptr++;
			*vptr=(dd1+dd2+dd3+dd4) >> 2;
			vptr++;
			*vptr=(dd2+dd4) >> 1;
			vptr+=(LINESIZE-1);
			*vptr=(dd3+dd4) >> 1;
			vptr+=(2-LINESIZE*2);
			d0+=1;
		}
		vptr0+=LINESIZE*3;
	}
}

//よくわかんないx2 - LCK
void pgBitBltN2(unsigned long x,unsigned long y,unsigned long h,unsigned long *d)
{
	unsigned long *v0;		//pointer to vram
	unsigned long xx,yy;
	unsigned long dx,dl[2];

	v0=(unsigned long *)pgGetVramAddr(x,y);
	d+=(SIZE_LINE*4+GUARD_LINE)/2;
	for (yy=h; yy>0; --yy) {
		for (xx=80; xx>0; --xx) {
			dx=*(d++);
//			dl[0]=( (dx&0x0000ffff)|((dx&0x0000ffff)<<16) );
//			dl[1]=( (dx&0xffff0000)|((dx&0xffff0000)>>16) );
			cpy2x(dl,dx);
			v0[LINESIZE/2]=dl[0];
			v0[LINESIZE/2+1]=dl[1];
			*(v0++)=dl[0];
			*(v0++)=dl[1];
		}
		v0+=(LINESIZE-160);
		d+=GUARD_LINE;
	}
}

//by z-rwt
void pgBitBltStScan(unsigned long x,unsigned long y,unsigned long h,unsigned long *d)
{
	unsigned long *v0;		//pointer to vram
	unsigned long xx,yy;
	unsigned long dx;

	v0=(unsigned long *)pgGetVramAddr(x,y);
	d+=(SIZE_LINE*4+GUARD_LINE)/2;
	for (yy=h; yy>0; --yy) {
		for (xx=80; xx>0; --xx) {
			dx=*(d++);
//			d0=( (dx&0x0000ffff)|((dx&0x0000ffff)<<16) );
//			d1=( (dx&0xffff0000)|((dx&0xffff0000)>>16) );
//			*(v0++)=d0;
//			*(v0++)=d1;
			cpy2x(v0,dx);
			v0+=2;
		}
		v0+=(LINESIZE-160);
		d+=GUARD_LINE;
	}
}

//by z-rwt
void pgBitBltSt2wotop(unsigned long x,unsigned long y,unsigned long h,unsigned long *d)
{
	unsigned long *v0;		//pointer to vram
	unsigned long xx,yy;
	unsigned long dx,dl[2];

	v0=(unsigned long *)pgGetVramAddr(x,y);
	d+=GUARD_LINE/2;
	for (yy=0; yy<16; yy++){
		for (xx=80; xx>0; --xx) {
			dx=*(d++);
//			d0=( (dx&0x0000ffff)|((dx&0x0000ffff)<<16) );
//			d1=( (dx&0xffff0000)|((dx&0xffff0000)>>16) );
//			*(v0++)=d0;
//			*(v0++)=d1;
			cpy2x(v0,dx);
			v0+=2;
		}
		v0+=(LINESIZE/2-160);
		d+=GUARD_LINE;
	}
	for (; yy<h; yy++) {
		for (xx=80; xx>0; --xx) {
			dx=*(d++);
//			dl[0]=( (dx&0x0000ffff)|((dx&0x0000ffff)<<16) );
//			dl[1]=( (dx&0xffff0000)|((dx&0xffff0000)>>16) );
			cpy2x(dl,dx);
			v0[LINESIZE/2]=dl[0];
			v0[LINESIZE/2+1]=dl[1];
			*(v0++)=dl[0];
			*(v0++)=dl[1];
		}
		v0+=(LINESIZE-160);
		d+=GUARD_LINE;
	}
}

//by z-rwt
void pgBitBltSt2wobot(unsigned long x,unsigned long y,unsigned long h,unsigned long *d)
{
	unsigned long *v0;		//pointer to vram
	unsigned long xx,yy;
	unsigned long dx,dl[2];

	v0=(unsigned long *)pgGetVramAddr(x,y);
	d+=GUARD_LINE/2;
	for (yy=0; yy<h-16; yy++){
		for (xx=80; xx>0; --xx) {
			dx=*(d++);
//			dl[0]=( (dx&0x0000ffff)|((dx&0x0000ffff)<<16) );
//			dl[1]=( (dx&0xffff0000)|((dx&0xffff0000)>>16) );
			cpy2x(dl,dx);
			v0[LINESIZE/2]=dl[0];
			v0[LINESIZE/2+1]=dl[1];
			*(v0++)=dl[0];
			*(v0++)=dl[1];
		}
		v0+=(LINESIZE-160);
		d+=GUARD_LINE;
	}
	for (; yy<h; yy++) {
		for (xx=80; xx>0; --xx) {
			dx=*(d++);
//			d0=( (dx&0x0000ffff)|((dx&0x0000ffff)<<16) );
//			d1=( (dx&0xffff0000)|((dx&0xffff0000)>>16) );
//			*(v0++)=d0;
//			*(v0++)=d1;
			cpy2x(v0,dx);
			v0+=2;
		}
		v0+=(LINESIZE/2-160);
		d+=GUARD_LINE;
	}
}

//Parallel blend
static inline unsigned long PBlend(unsigned long c0, unsigned long c1)
{
	return (c0 & c1) + (((c0 ^ c1) & 0x7bde7bde) >> 1);
}

//2x Fit
void pgBitBltSt2Fix(unsigned long x,unsigned long y,unsigned long h,unsigned long mag,const unsigned short *d)
{
	unsigned long	*vptr0;		//pointer to vram
	unsigned long	*vptr;		//pointer to vram
	unsigned long	xx, yy;
	unsigned short	er, f, hf;
	unsigned long	*dl;

	f = hf = 0;
	er = SCREEN_HEIGHT;
	vptr0 = (unsigned long*)pgGetVramAddr(x, 0);
	d+=GUARD_LINE;
	for(yy = 0; yy < SCREEN_HEIGHT; yy++) {
		vptr = vptr0;
		dl = (unsigned long *)d;
		if(hf == 0) {
			for(xx = 80; xx > 0; xx--) {
				cpy2x(vptr, *dl++);
				vptr+=2;
			}
		} else {
			for(xx = 80; xx > 0; xx--) {
				cpy2x(vptr, PBlend(*(dl-SIZE_LINE/2), *dl));
				dl++;
				vptr+=2;
			}
			hf = 0;
		}
		vptr0 += LINESIZE/2;
		er += 15;
		if(er > SCREEN_HEIGHT - 3 && f == 0) {
			er -= SCREEN_HEIGHT - 2;
			f++;
			hf = 1;
		}
		f++;
		if(f > 1) {
			f -= 2;
			d += SIZE_LINE;
		}
	}
}

//Full
void pgBitBltStFull(unsigned long x,unsigned long y,unsigned long h,unsigned long mag,const unsigned short *d)
{
	unsigned long	*vptr0;		//pointer to vram
	unsigned long	xx, yy;
	unsigned short	er, f, hf;
	unsigned long	*dl;

	f = hf = 0;
	er = SCREEN_HEIGHT;
	vptr0 = (unsigned long*)pgGetVramAddr(0, 0);
	d+=GUARD_LINE;
	for(yy = SCREEN_HEIGHT; yy > 0 ; yy--) {
		dl = (unsigned long *)d;
		if(hf == 0) {
			for(xx = 80; xx > 0; xx--) {
				cpy3x(vptr0, *dl++);
				vptr0+=3;
			}
		} else {
			for(xx = 80; xx > 0; xx--) {
				cpy3x(vptr0, PBlend(*(dl-SIZE_LINE/2), *dl));
				dl++;
				vptr0+=3;
			}
			hf = 0;
		}
		vptr0 += (LINESIZE -160*3)/2;
		er += 15;
		if(er > SCREEN_HEIGHT - 3 && f == 0) {
			er -= SCREEN_HEIGHT - 2;
			f++;
			hf = 1;
		}
		f++;
		if(f > 1) {
			f -= 2;
			d += SIZE_LINE;
		}
	}
}

void pgBitBltGe(int x, int y, int w, int h, const unsigned short *d)
{
	static int qid = -1;

	ScreenVertex[2] = x;
	ScreenVertex[3] = y;
	ScreenVertex[7] = x+w-1;
	ScreenVertex[8] = y+h-1;
	GEcmd[ 0] = 0x9C000000UL | ((u32)(unsigned char *)pgGetVramAddr( 0, 0 ) & 0x00FFFFFF);
	GEcmd[ 2] = 0xA0000000UL | ((u32)(unsigned char *)d & 0x00FFFFFF);
	GEcmd[ 3] = 0xA8000000UL | (((u32)(unsigned char *)d & 0xFF000000) >> 8) | SIZE_LINE;
	sceKernelDcacheWritebackAll();
	qid = sceGeListEnQueue(&GEcmd[0], &GEcmd[14], gecbid, NULL);
	if (qid >= 0) sceGeListSync(qid, 0);
}

void pgPutChar(unsigned long x,unsigned long y,unsigned long color,unsigned long bgcolor,unsigned char ch,char drawfg,char drawbg,char mag)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	const unsigned char *cfont;		//pointer to font
	unsigned long cx,cy;
	unsigned long b;
	char mx,my;

	cfont=font+ch*8;
	vptr0=(unsigned char *)pgGetVramAddr(x,y);
	for (cy=0; cy<8; cy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			b=0x80;
			for (cx=0; cx<8; cx++) {
				for (mx=0; mx<mag; mx++) {
					if ((*cfont&b)!=0) {
						if (drawfg) *(unsigned short *)vptr=color;
					} else {
						if (drawbg) *(unsigned short *)vptr=bgcolor;
					}
					vptr+=PIXELSIZE*2;
				}
				b=b>>1;
			}
			vptr0+=LINESIZE*2;
		}
		cfont++;
	}
}


void pgScreenFrame(long mode,long frame)
{
	pg_screenmode=mode;
	frame=(frame?1:0);
	pg_showframe=frame;
	if (mode==0) {
		//screen off
		pg_drawframe=frame;
		sceDisplaySetFrameBuf(0,0,0,1);
	} else if (mode==1) {
		//show/draw same
		pg_drawframe=frame;
		sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	} else if (mode==2) {
		//show/draw different
		pg_drawframe=(frame?0:1);
		sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	}
}


void pgScreenFlip()
{
    test_available_memory();
	pg_showframe=(pg_showframe?0:1);
	pg_drawframe=(pg_drawframe?0:1);
	sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,0);
}


void pgScreenFlipV()
{
	pgWaitV();
	pgScreenFlip();
}

// by kwn
void Draw_Char_Hankaku(int x,int y,const unsigned char c,int col) {
	unsigned short *vr;
	unsigned char  *fnt;
	unsigned char  pt;
	unsigned char ch;
	int x1,y1;

	ch = c;

	// mapping
	if (ch<0x20)
		ch = 0;
	else if (ch<0x80)
		ch -= 0x20;
	else if (ch<0xa0)
		ch = 0;
	else
		ch -= 0x40;

	fnt = (unsigned char *)&hankaku_font10[ch*10];

	// draw
	vr = (unsigned short *)pgGetVramAddr(x,y);
	for(y1=0;y1<10;y1++) {
		pt = *fnt++;
		for(x1=0;x1<5;x1++) {
			if (pt & 1)
				*vr = col;
			vr++;
			pt = pt >> 1;
		}
		vr += LINESIZE-5;
	}
}

// by kwn
void Draw_Char_Zenkaku(int x,int y,const unsigned char u,unsigned char d,int col) {
	// ELISA100.FNTに存在しない文字
	const unsigned short font404[] = {
		0xA2AF, 11,
		0xA2C2, 8,
		0xA2D1, 11,
		0xA2EB, 7,
		0xA2FA, 4,
		0xA3A1, 15,
		0xA3BA, 7,
		0xA3DB, 6,
		0xA3FB, 4,
		0xA4F4, 11,
		0xA5F7, 8,
		0xA6B9, 8,
		0xA6D9, 38,
		0xA7C2, 15,
		0xA7F2, 13,
		0xA8C1, 720,
		0xCFD4, 43,
		0xF4A5, 1030,
		0,0
	};
	unsigned short *vr;
	unsigned short *fnt;
	unsigned short pt;
	int x1,y1;

	unsigned long n;
	unsigned short code;
	int i;

	// SJISコードの生成
	code = u;
	code = (code<<8) + d;

	// SJISからEUCに変換
	if(code >= 0xE000) code-=0x4000;
	code = ((((code>>8)&0xFF)-0x81)<<9) + (code&0x00FF);
	if((code & 0x00FF) >= 0x80) code--;
	if((code & 0x00FF) >= 0x9E) code+=0x62;
	else code-=0x40;
	code += 0x2121 + 0x8080;

	// EUCから恵梨沙フォントの番号を生成
	n = (((code>>8)&0xFF)-0xA1)*(0xFF-0xA1)
		+ (code&0xFF)-0xA1;
	i=0;
	while(font404[i]) {
		if(code >= font404[i]) {
			if(code <= font404[i]+font404[i+1]-1) {
				n = -1;
				break;
			} else {
				n-=font404[i+1];
			}
		}
		i+=2;
	}
	fnt = (unsigned short *)&zenkaku_font10[n*10];

	// draw
	vr = (unsigned short *)pgGetVramAddr(x,y);
	for(y1=0;y1<10;y1++) {
		pt = *fnt++;
		for(x1=0;x1<10;x1++) {
			if (pt & 1)
				*vr = col;
			vr++;
			pt = pt >> 1;
		}
		vr += LINESIZE-10;
	}
}

// by kwn
void mh_print(int x,int y,const char *msg,int col) {
	int xx = x;
	unsigned char ch = 0,bef = 0, *str=(unsigned char*)msg;

	while(*str != 0) {
		ch = *str++;
		if (bef!=0) {
			Draw_Char_Zenkaku(x,y,bef,ch,col);
			x+=10;
			bef=0;
		} else {
			if (((ch>=0x80) && (ch<0xa0)) || (ch>=0xe0)) {
				bef = ch;
			} else {
				if (ch=='\n'){
					x=xx;
					y+=10;
				}else{
					Draw_Char_Hankaku(x,y,ch,col);
					x+=5;
				}
			}
		}
		if (x>=480) break;
	}
}

u32 new_pad;
u32 old_pad;
u32 now_pad;
ctrl_data_t paddata;

void readpad(void)
{
	static int n=0;
	ctrl_data_t paddata;

	sceCtrlReadBufferPositive(&paddata, 1);
	// kmg
	// Analog pad state
	if (paddata.analog[CTRL_ANALOG_Y] == 0xff) paddata.buttons=CTRL_DOWN;  // DOWN
	if (paddata.analog[CTRL_ANALOG_Y] == 0x00) paddata.buttons=CTRL_UP;    // UP
	if (paddata.analog[CTRL_ANALOG_X] == 0x00) paddata.buttons=CTRL_LEFT;  // LEFT
	if (paddata.analog[CTRL_ANALOG_X] == 0xff) paddata.buttons=CTRL_RIGHT; // RIGHT

	now_pad = paddata.buttons;
	new_pad = now_pad & ~old_pad;
	if(old_pad==now_pad){
		n++;
		if(n>=25){
			new_pad=now_pad;
			n = 20;
		}
	}else{
		n=0;
		old_pad = now_pad;
	}
}

/******************************************************************************/


void pgcLocate(unsigned long x, unsigned long y)
{
	if (x>=CMAX_X) x=0;
	if (y>=CMAX_Y) y=0;
	pgc_csr_x[pg_drawframe?1:0]=x;
	pgc_csr_y[pg_drawframe?1:0]=y;
}


void pgcColor(unsigned long fg, unsigned long bg)
{
	pgc_fgcolor[pg_drawframe?1:0]=fg;
	pgc_bgcolor[pg_drawframe?1:0]=bg;
}


void pgcDraw(char drawfg, char drawbg)
{
	pgc_fgdraw[pg_drawframe?1:0]=drawfg;
	pgc_bgdraw[pg_drawframe?1:0]=drawbg;
}


void pgcSetmag(char mag)
{
	pgc_mag[pg_drawframe?1:0]=mag;
}

void pgcCls()
{
	pgFillvram(pgc_bgcolor[pg_drawframe]);
	pgcLocate(0,0);
}

void pgcPutchar_nocontrol(const char ch)
{
	pgPutChar(pgc_csr_x[pg_drawframe]*8, pgc_csr_y[pg_drawframe]*8, pgc_fgcolor[pg_drawframe], pgc_bgcolor[pg_drawframe], ch, pgc_fgdraw[pg_drawframe], pgc_bgdraw[pg_drawframe], pgc_mag[pg_drawframe]);
	pgc_csr_x[pg_drawframe]+=pgc_mag[pg_drawframe];
	if (pgc_csr_x[pg_drawframe]>CMAX_X-pgc_mag[pg_drawframe]) {
		pgc_csr_x[pg_drawframe]=0;
		pgc_csr_y[pg_drawframe]+=pgc_mag[pg_drawframe];
		if (pgc_csr_y[pg_drawframe]>CMAX_Y-pgc_mag[pg_drawframe]) {
			pgc_csr_y[pg_drawframe]=CMAX_Y-pgc_mag[pg_drawframe];
//			pgMoverect(0,pgc_mag[pg_drawframe]*8,SCREEN_WIDTH,SCREEN_HEIGHT-pgc_mag[pg_drawframe]*8,0,0);
		}
	}
}

void pgcPutchar(const char ch)
{
	if (ch==0x0d) {
		pgc_csr_x[pg_drawframe]=0;
		return;
	}
	if (ch==0x0a) {
		if ((++pgc_csr_y[pg_drawframe])>=CMAX_Y) {
			pgc_csr_y[pg_drawframe]=CMAX_Y-1;
//			pgMoverect(0,8,SCREEN_WIDTH,SCREEN_HEIGHT-8,0,0);
		}
		return;
	}
	pgcPutchar_nocontrol(ch);
}

void pgcPuthex2(const unsigned long s)
{
	char ch;
	ch=((s>>4)&0x0f);
	pgcPutchar((ch<10)?(ch+0x30):(ch+0x40-9));
	ch=(s&0x0f);
	pgcPutchar((ch<10)?(ch+0x30):(ch+0x40-9));
}


void pgcPuthex8(const unsigned long s)
{
	pgcPuthex2(s>>24);
	pgcPuthex2(s>>16);
	pgcPuthex2(s>>8);
	pgcPuthex2(s);
}

/******************************************************************************/



void pgiInit()
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(1);
}

/******************************************************************************/

int pgaOutBlocking(unsigned long channel,unsigned long vol1,unsigned long vol2,void *buf);

#define PGA_CHANNELS 1
#define PGA_SAMPLES 256
#define MAXVOLUME 0x8000

int pga_ready=0;
int pga_handle[PGA_CHANNELS];

short pga_sndbuf[PGA_CHANNELS][2][PGA_SAMPLES][2];


void (*pga_channel_callback[PGA_CHANNELS])(void *buf, unsigned long reqn);

int pga_threadhandle[PGA_CHANNELS];


volatile int pga_terminate=0;


static int pga_channel_thread(int args, void *argp)
{
	volatile int bufidx=0;
	int channel=*(int *)argp;

	while (pga_terminate==0) {
		void *bufptr=&pga_sndbuf[channel][bufidx];
		void (*callback)(void *buf, unsigned long reqn);
		callback=pga_channel_callback[channel];
		if (callback) {
			callback(bufptr,PGA_SAMPLES);
		} else {
			unsigned long *ptr=bufptr;
			int i;
			for (i=0; i<PGA_SAMPLES; ++i) *(ptr++)=0;
		}
		pgaOutBlocking(channel,0x8000,0x8000,bufptr);
		bufidx=(bufidx?0:1);
	}
	sceKernelExitThread(0);
	return 0;
}

/*
void pga_channel_thread_callback(int channel, void *buf, unsigned long reqn)
{
	void (*callback)(void *buf, unsigned long reqn);
	callback=pga_channel_callback[channel];
}
*/

void pgaSetChannelCallback(int channel, void *callback)
{
	pga_channel_callback[channel]=callback;
}



/******************************************************************************/

int pgaInit()
{
	int i,ret;
	int failed=0;
	char str[32];

	pga_terminate=0;
	pga_ready=0;

	for (i=0; i<PGA_CHANNELS; i++) {
		pga_handle[i]=-1;
		pga_threadhandle[i]=-1;
		pga_channel_callback[i]=0;
	}
	for (i=0; i<PGA_CHANNELS; i++) {
		if ((pga_handle[i]=sceAudioChReserve(-1,PGA_SAMPLES,0))<0) failed=1;
	}
	if (failed) {
		for (i=0; i<PGA_CHANNELS; i++) {
			if (pga_handle[i]!=-1) sceAudioChRelease(pga_handle[i]);
			pga_handle[i]=-1;
		}
		return -1;
	}
	pga_ready=1;

	strcpy(str,"pgasnd0");
	for (i=0; i<PGA_CHANNELS; i++) {
		str[6]='0'+i;
		pga_threadhandle[i]=sceKernelCreateThread(str,(SceKernelThreadEntry)&pga_channel_thread,0x12,0x10000,0,NULL);
		if (pga_threadhandle[i]<0) {
			pga_threadhandle[i]=-1;
			failed=1;
			break;
		}
		ret=sceKernelStartThread(pga_threadhandle[i],sizeof(i),&i);
		if (ret!=0) {
			failed=1;
			break;
		}
	}
	if (failed) {
		pga_terminate=1;
		for (i=0; i<PGA_CHANNELS; i++) {
			if (pga_threadhandle[i]!=-1) {
				sceKernelWaitThreadEnd(pga_threadhandle[i],NULL);
				sceKernelDeleteThread(pga_threadhandle[i]);
			}
			pga_threadhandle[i]=-1;
		}
		pga_ready=0;
		return -1;
	}
	return 0;
}


void pgaTermPre()
{
	pga_ready=0;
	pga_terminate=1;
}


void pgaTerm()
{
	int i;
	pga_ready=0;
	pga_terminate=1;

	for (i=0; i<PGA_CHANNELS; i++) {
		if (pga_threadhandle[i]!=-1) {
			sceKernelWaitThreadEnd(pga_threadhandle[i],NULL);
			sceKernelDeleteThread(pga_threadhandle[i]);
		}
		pga_threadhandle[i]=-1;
	}

	for (i=0; i<PGA_CHANNELS; i++) {
		if (pga_handle[i]!=-1) {
			sceAudioChRelease(pga_handle[i]);
			pga_handle[i]=-1;
		}
	}
}



int pgaOutBlocking(unsigned long channel,unsigned long vol1,unsigned long vol2,void *buf)
{
	if (!pga_ready) return -1;
	if (channel>=PGA_CHANNELS) return -1;
	if (vol1>MAXVOLUME) vol1=MAXVOLUME;
	if (vol2>MAXVOLUME) vol2=MAXVOLUME;
	return sceAudioOutputPannedBlocking(pga_handle[channel],vol1,vol2,buf);
}

//バッファは64バイト境界じゃなくても大丈夫みたい
//[0]が左、[1]が右
//サンプル速度は44100
//vol1が左

/******************************************************************************/
#ifdef USE_GPU
void Ge_Finish_Callback(int id, void *arg)
{
}

void InitDisplay()
{
	ScreenVertex[0] = GUARD_LINE;
	ScreenVertex[1] = 0;
	ScreenVertex[2] = 0;
	ScreenVertex[3] = 0;
	ScreenVertex[4] = 0;
	ScreenVertex[5] = 160+GUARD_LINE;
	ScreenVertex[6] = 144;
	ScreenVertex[7] = 0;
	ScreenVertex[8] = 0;
	ScreenVertex[9] = 0;
	// Set Draw Buffer
	GEcmd[ 0] = 0x9C000000UL | ((u32)(unsigned char *)pgGetVramAddr( 0, 0 ) & 0x00FFFFFF);
	GEcmd[ 1] = 0x9D000000UL | (((u32)(unsigned char *)pgGetVramAddr( 0, 0 ) & 0xFF000000) >> 8) | 512;
	// Set Tex Buffer
	GEcmd[ 2] = 0xA0000000UL | ((u32)(unsigned char *)vframe & 0x00FFFFFF);
	GEcmd[ 3] = 0xA8000000UL | (((u32)(unsigned char *)vframe & 0xFF000000) >> 8) | SIZE_LINE;
	// Tex size
	GEcmd[ 4] = 0xB8000000UL | (8 << 8) | 8;
	// Tex Flush
	GEcmd[ 5] = 0xCB000000UL;
	// Set Vertex
	GEcmd[ 6] = 0x12000000UL | (1 << 23) | (0 << 11) | (0 << 9) | (2 << 7) | (0 << 5) | (0 << 2) | 2;
	GEcmd[ 7] = 0x10000000UL;
	GEcmd[ 8] = 0x02000000UL;
	GEcmd[ 9] = 0x10000000UL | (((u32)(void *)ScreenVertex & 0xFF000000) >> 8);
	GEcmd[10] = 0x01000000UL | ((u32)(void *)ScreenVertex & 0x00FFFFFF);
	// Draw Vertex
	GEcmd[11] = 0x04000000UL | (6 << 16) | 2;
	// List End
	GEcmd[12] = 0x0F000000UL;
	GEcmd[13] = 0x0C000000UL;
	GEcmd[14] = 0;
	GEcmd[15] = 0;
	//GeFlip = 0;
}

void pgGeInit()
{
	static unsigned int GeInit[] = {
	0x01000000, 0x02000000,
	0x10000000, 0x12000000, 0x13000000, 0x15000000, 0x16000000, 0x17000000,
	0x18000000, 0x19000000, 0x1A000000, 0x1B000000, 0x1C000000, 0x1D000000,
	0x1E000000, 0x1F000000,
	0x20000000, 0x21000000, 0x22000000, 0x23000000, 0x24000000, 0x25000000,
	0x26000000, 0x27000000, 0x28000000, 0x2A000000, 0x2B000000, 0x2C000000,
	0x2D000000, 0x2E000000, 0x2F000000,
	0x30000000, 0x31000000, 0x32000000, 0x33000000, 0x36000000, 0x37000000,
	0x38000000, 0x3A000000, 0x3B000000, 0x3C000000, 0x3D000000, 0x3E000000,
	0x3F000000,
	0x40000000, 0x41000000, 0x42000000, 0x43000000, 0x44000000, 0x45000000,
	0x46000000, 0x47000000, 0x48000000, 0x49000000, 0x4A000000, 0x4B000000,
	0x4C000000, 0x4D000000,
	0x50000000, 0x51000000, 0x53000000, 0x54000000, 0x55000000, 0x56000000,
	0x57000000, 0x58000000, 0x5B000000, 0x5C000000, 0x5D000000, 0x5E000000,
	0x5F000000,
	0x60000000, 0x61000000, 0x62000000, 0x63000000, 0x64000000, 0x65000000,
	0x66000000, 0x67000000, 0x68000000, 0x69000000, 0x6A000000, 0x6B000000,
	0x6C000000, 0x6D000000, 0x6E000000, 0x6F000000,
	0x70000000, 0x71000000, 0x72000000, 0x73000000, 0x74000000, 0x75000000,
	0x76000000, 0x77000000, 0x78000000, 0x79000000, 0x7A000000, 0x7B000000,
	0x7C000000, 0x7D000000, 0x7E000000, 0x7F000000,
	0x80000000, 0x81000000, 0x82000000, 0x83000000, 0x84000000, 0x85000000,
	0x86000000, 0x87000000, 0x88000000, 0x89000000, 0x8A000000, 0x8B000000,
	0x8C000000, 0x8D000000, 0x8E000000, 0x8F000000,
	0x90000000, 0x91000000, 0x92000000, 0x93000000, 0x94000000, 0x95000000,
	0x96000000, 0x97000000, 0x98000000, 0x99000000, 0x9A000000, 0x9B000000,
	0x9C000000, 0x9D000000, 0x9E000000, 0x9F000000,
	0xA0000000, 0xA1000000, 0xA2000000, 0xA3000000, 0xA4000000, 0xA5000000,
	0xA6000000, 0xA7000000, 0xA8000000, 0xA9000000, 0xAA000000, 0xAB000000,
	0xAC000000, 0xAD000000, 0xAE000000, 0xAF000000,
	0xB0000000, 0xB1000000, 0xB2000000, 0xB3000000, 0xB4000000, 0xB5000000,
	0xB8000000, 0xB9000000, 0xBA000000, 0xBB000000, 0xBC000000, 0xBD000000,
	0xBE000000, 0xBF000000,
	0xC0000000, 0xC1000000, 0xC2000000, 0xC3000000, 0xC4000000, 0xC5000000,
	0xC6000000, 0xC7000000, 0xC8000000, 0xC9000000, 0xCA000000, 0xCB000000,
	0xCC000000, 0xCD000000, 0xCE000000, 0xCF000000,
	0xD0000000, 0xD2000000, 0xD3000000, 0xD4000000, 0xD5000000, 0xD6000000,
	0xD7000000, 0xD8000000, 0xD9000000, 0xDA000000, 0xDB000000, 0xDC000000,
	0xDD000000, 0xDE000000, 0xDF000000,
	0xE0000000, 0xE1000000, 0xE2000000, 0xE3000000, 0xE4000000, 0xE5000000,
	0xE6000000, 0xE7000000, 0xE8000000, 0xE9000000, 0xEB000000, 0xEC000000,
	0xEE000000,
	0xF0000000, 0xF1000000, 0xF2000000, 0xF3000000, 0xF4000000, 0xF5000000,
	0xF6000000,	0xF7000000, 0xF8000000, 0xF9000000,
	0x0F000000, 0x0C000000};

	int qid;
	sceKernelDcacheWritebackAll();
	qid = sceGeListEnQueue(&GeInit[0], 0, -1, 0);
	sceGeListSync(qid, 0);
	
	static unsigned int GEcmd[64];
	// Draw Area
	GEcmd[ 0] = 0x15000000UL | (0 << 10) | 0;
	GEcmd[ 1] = 0x16000000UL | (271 << 10) | 479;
	// Tex Enable
	GEcmd[ 2] = 0x1E000000UL | 1;
	// Viewport
	GEcmd[ 3] = 0x42000000UL | (((int)((float)(480)) >> 8) & 0x00FFFFFF);
	GEcmd[ 4] = 0x43000000UL | (((int)((float)(-272)) >> 8) & 0x00FFFFFF);
	GEcmd[ 5] = 0x44000000UL | (((int)((float)(50000)) >> 8) & 0x00FFFFFF);
	GEcmd[ 6] = 0x45000000UL | (((int)((float)(2048)) >> 8) & 0x00FFFFFF);
	GEcmd[ 7] = 0x46000000UL | (((int)((float)(2048)) >> 8) & 0x00FFFFFF);
	GEcmd[ 8] = 0x47000000UL | (((int)((float)(60000)) >> 8) & 0x00FFFFFF);
	GEcmd[ 9] = 0x4C000000UL | (1024 << 4);
	GEcmd[10] = 0x4D000000UL | (1024 << 4);
	// Model Color
	GEcmd[11] = 0x54000000UL;
	GEcmd[12] = 0x55000000UL | 0xFFFFFF;
	GEcmd[13] = 0x56000000UL | 0xFFFFFF;
	GEcmd[14] = 0x57000000UL | 0xFFFFFF;
	GEcmd[15] = 0x58000000UL | 0xFF;
	// Depth Buffer
	GEcmd[16] = 0x9E000000UL | 0x88000;
	GEcmd[17] = 0x9F000000UL | (0x44 << 16) | 512;
	// Tex
	GEcmd[18] = 0xC2000000UL | (0 << 16) | (0 << 8) | 0;
	GEcmd[19] = 0xC3000000UL | 1;
	GEcmd[20] = 0xC6000000UL | (1 << 8) | 1;
	GEcmd[21] = 0xC7000000UL | (1 << 8) | 1;
	GEcmd[22] = 0xC9000000UL | (0 << 16) | (0 << 8) | 3;
	// Pixel Format
	GEcmd[23] = 0xD2000000UL | 1;
	// Scissor
	GEcmd[24] = 0xD4000000UL | (0 << 10) | 0;
	GEcmd[25] = 0xD5000000UL | (271 << 10) | 479;
	// Depth
	GEcmd[26] = 0xD6000000UL | 10000;
	GEcmd[27] = 0xD7000000UL | 50000;
	// List End
	GEcmd[28] = 0x0F000000UL;
	GEcmd[29] = 0x0C000000UL;
	GEcmd[30] = 0;
	sceKernelDcacheWritebackAll();
	qid = sceGeListEnQueue(&GEcmd[0], &GEcmd[30], -1, 0);
	sceGeListSync(qid, 0);
	
	GeCB gecb;
	gecb.signal_func = NULL;
	gecb.signal_arg = NULL;
	gecb.finish_func = Ge_Finish_Callback;
	gecb.finish_arg = NULL;
	gecbid = sceGeSetCallback(&gecb);
	
	InitDisplay();
}
#endif
/******************************************************************************/

void pgInit(void)
{
	sceDisplaySetMode(0,SCREEN_WIDTH,SCREEN_HEIGHT);
	
	pgScreenFrame(0,1);
	pgcLocate(0,0);
	pgcColor(0xffff,0x0000);
	pgcDraw(1,1);
	pgcSetmag(1);
	pgScreenFrame(0,0);
	pgcLocate(0,0);
	pgcColor(0xffff,0x0000);
	pgcDraw(1,1);
	pgcSetmag(1);
	
	pgiInit();
	pgaInit();
#ifdef USE_GPU
	pgGeInit();
#endif
}
