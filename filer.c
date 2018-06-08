#include "main.h"
#include "gz.h"
#include "lib/zlibInterface.h"

SceIoDirent files[MAX_ENTRY];
SceIoDirent *sortfiles[MAX_ENTRY];
int nfiles;
char path_files[MAX_PATH]={0};

SceIoDirent zip_files[MAX_ENTRY];
SceIoDirent *zip_sortfiles[MAX_ENTRY];
int zip_nfiles;
char path_inzip[MAX_PATH]={0};

// 拡張子管理用
const struct {
	char *szExt;
	int nExtId;
} stExtentions[] = {
	{"gb",EXT_GB},
	{"gbc",EXT_GB},
	{"sgb",EXT_GB},
	{"gz",EXT_GZ},
	{"zip",EXT_ZIP},
	{"tch",EXT_TCH},
	{NULL, EXT_UNKNOWN}
};
int getExtId(const char *szFilePath) {
	char *pszExt;

	if ((pszExt = strrchr(szFilePath, '.'))) {
		pszExt++;
		int i;
		for (i = 0; stExtentions[i].nExtId != EXT_UNKNOWN; i++) {
			if (!stricmp(stExtentions[i].szExt,pszExt)) {
				return stExtentions[i].nExtId;
			}
		}
	}

	return EXT_UNKNOWN;
}

// Unzip 対応 by ruka

// コールバック受け渡し用
typedef struct {
	byte *p_rom_image;			// pointer to rom image
	long rom_size;				// rom size
	char szFileName[MAX_PATH];	// extracted file name
}ROM_INFO, *LPROM_INFO;

// せっかくなのでプログレスでも出してみます
void draw_load_rom_progress(unsigned long ulExtractSize, unsigned long ulCurrentPosition)
{
	int nPer = 100 * ulExtractSize / ulCurrentPosition;
	static int nOldPer = 0;
	if (nOldPer == (nPer & 0xFFFFFFFE)) {
		return ;
	}
	nOldPer = nPer;
	if(bBitmap)
		pgBitBlt(0,0,480,272,1,bgBitmap);
	else
		pgFillvram(setting.color[0]);
	// プログレス
	pgDrawFrame(89,121,391,141,setting.color[1]);
	pgFillBox(90,123, 90+nPer*3, 139,setting.color[1]);
	// ％
	char szPer[16];
	sprintf(szPer,"%d%%",nPer);
	pgPrint(28,16,setting.color[3],szPer);
	// pgScreenFlipV()を使うとpgWaitVが呼ばれてしまうのでこちらで。
	// プログレスだからちらついても良いよね～
	pgScreenFlip();
}

// Unzip コールバック
int funcUnzipCallback(int nCallbackId, unsigned long ulExtractSize, unsigned long ulCurrentPosition,
                      const void *pData, unsigned long ulDataSize, unsigned long ulUserData)
{
    const char *pszFileName;
    int nExtId;
    const unsigned char *pbData;
    LPROM_INFO pRomInfo = (LPROM_INFO)ulUserData;

    switch(nCallbackId) {
    case UZCB_FIND_FILE:
		pszFileName = (const char *)pData;
		
		if (path_inzip[0] && strcmp(pszFileName,path_inzip))
			break;
		
		nExtId = getExtId(pszFileName);
		// 拡張子がGB・GBC・SGBなら展開
		if (nExtId == EXT_GB) {
			// 展開する名前、rom sizeを覚えておく
			strcpy(pRomInfo->szFileName, pszFileName);
			pRomInfo->rom_size = ulExtractSize;
			return UZCBR_OK;
		}
        break;
    case UZCB_EXTRACT_PROGRESS:
		pbData = (const unsigned char *)pData;
		// 展開されたデータを格納しよう
		memcpy(pRomInfo->p_rom_image + ulCurrentPosition, pbData, ulDataSize);
		draw_load_rom_progress(ulCurrentPosition + ulDataSize, ulExtractSize);
		return UZCBR_OK;
        break;
    default: // unknown...
		pgFillvram(RGB(255,0,0));
		mh_print(0,0,"Unzip fatal error.",0xFFFF);
		pgScreenFlipV();
        break;
    }
    return UZCBR_PASS;
}

// load rom image by ruka
long load_rom(const char *szRomPath)
{
	char name[MAX_NAME], *p;
	long lReadSize=0;
	ROM_INFO stRomInfo;
	int nRet;
	int nExtId = getExtId(szRomPath);

	switch(nExtId) {
	case EXT_GB:{	// "gb" "gbc" "sgb"
		p = strrchr(szRomPath, '/');
		if (!p)
			return 0;
		strcpy(name, p+1);

		int fd = sceIoOpen(szRomPath, SCE_O_RDONLY, 644);
		lReadSize = sceIoRead(fd, rom_image, MAX_ROM_SIZE);
		sceIoClose(fd);
		break;

	}case EXT_GZ:{	// "gz"
		gzFile fd = gzopen(szRomPath, "r");
		if (!gzGetOrigName(szRomPath, name, fd)){
			gzclose(fd);
			return 0;
		}else if (getExtId(name)!=EXT_GB){
			gzclose(fd);
			return 0;
		}
		lReadSize = gzread(fd, rom_image, MAX_ROM_SIZE);
		gzclose(fd);
		break;

	}case EXT_ZIP:	// "zip"
		if (path_inzip[0]){
			if (getExtId(path_inzip)!=EXT_GB)
				return 0;
			p = strrchr(path_inzip, '/');
			if (!p)
				p = path_inzip;
			else
				p++;
			strcpy(name, p);
		}else{
			p = strrchr(szRomPath, '/');
			if (!p)
				return 0;
			strcpy(name, p+1);
		}

		stRomInfo.p_rom_image = rom_image;
		stRomInfo.rom_size = 0;
		memset(stRomInfo.szFileName, 0x00, sizeof(stRomInfo.szFileName));
		// Unzipコールバックセット
		Unzip_setCallback(funcUnzipCallback);
		// Unzip展開する
	    nRet = Unzip_execExtract(szRomPath, (unsigned long)&stRomInfo);
		if (nRet != UZEXR_OK) {
			// 読み込み失敗！ - このコードでは、UZEXR_CANCELもここに来て
			// しまうがコールバックでキャンセルしてないので無視
			lReadSize = 0;
			pgFillvram(RGB(255,0,0));
			mh_print(0,0,"Unzip fatal error.",0xFFFF);
			pgScreenFlipV();
		}
		lReadSize = stRomInfo.rom_size;
		break;

	default:
		return 0;
	}
	
	if(lReadSize){
		strcpy(RomName, name);
		p = strrchr(RomName, '.');
		if(p)
			*p = 0;
	}
	
	return lReadSize;
}

////////////////////////////////////////////////////////////////////////
// クイックソート
// AC add start
void SJISCopy(SceIoDirent *a, unsigned char *file)
{
	unsigned char ca;
	int i;
	int len=strlen(a->name);
	
	for(i=0;i<=len;i++){
		ca = a->name[i];
		if (((0x81 <= ca)&&(ca <= 0x9f))
		|| ((0xe0 <= ca)&&(ca <= 0xef))){
			file[i++] = ca;
			file[i] = a->name[i];
		}
		else{
			if(ca>='a' && ca<='z') ca-=0x20;
			file[i] = ca;
		}
	}

}
int cmpFile(SceIoDirent *a, SceIoDirent *b)
{
    unsigned char file1[0x108];
    unsigned char file2[0x108];
	unsigned char ca, cb;
	int i, n, ret;

	if(a->type==b->type){
		SJISCopy(a, file1);
		SJISCopy(b, file2);
		n=strlen((char*)file1);
		for(i=0; i<=n; i++){
			ca=file1[i]; cb=file2[i];
			ret = ca-cb;
			if(ret!=0) return ret;
		}
		return 0;
	}
	
	if(a->type & TYPE_DIR)	return -1;
	else					return 1;
}
// AC add end

void sort_files(SceIoDirent **a, int left, int right) {
	SceIoDirent *tmp, *pivot;
	int i, p;
	
	if (left < right) {
		pivot = a[left];
		p = left;
		for (i=left+1; i<=right; i++) {
			if (cmpFile(a[i],pivot)<0){
				p=p+1;
				tmp=a[p];
				a[p]=a[i];
				a[i]=tmp;
			}
		}
		a[left] = a[p];
		a[p] = pivot;
		sort_files(a, left, p-1);
		sort_files(a, p+1, right);
	}
}
////////////////////////////////////////////////////////////////////////

void getDir(const char *path, u32 ext) {
	int fd, b=0;
	
	nfiles = 0;
	
	if(strcmp(path,"ms0:/")){
		strcpy(files[0].name,"..");
		files[0].type = TYPE_DIR;
		sortfiles[0] = files;
		nfiles = 1;
		b=1;
	}
	
	strcpy(path_files, path);
	fd = sceIoDopen(path);
	while(nfiles<MAX_ENTRY){
		memset(&files[nfiles], 0x00, sizeof(SceIoDirent));
		if(sceIoDread(fd, &files[nfiles])<=0) break;
		if(files[nfiles].name[0] == '.') continue;
		if(files[nfiles].type == TYPE_DIR){
			strcat(files[nfiles].name, "/");
			sortfiles[nfiles] = files + nfiles;
			nfiles++;
		}else if(getExtId(files[nfiles].name) & ext){
			sortfiles[nfiles] = files + nfiles;
			nfiles++;
		}
	}
	sceIoDclose(fd);
	if(b)
		sort_files(sortfiles+1, 0, nfiles-2);
	else
		sort_files(sortfiles, 0, nfiles-1);
}

int getZipDirCallback(int nCallbackId, unsigned long ulExtractSize, unsigned long ulCurrentPosition,
                      const void *pData, unsigned long ulDataSize, unsigned long ulUserData)
{
	const char *pszFileName = (const char *)pData;
	
	switch(nCallbackId) {
	case UZCB_FIND_FILE:
		if(getExtId(pszFileName)==EXT_GB){
			strcpy(zip_files[zip_nfiles].name, pszFileName);
			zip_sortfiles[zip_nfiles] = zip_files + zip_nfiles;
			zip_nfiles++;
		}
		if(zip_nfiles >= MAX_ENTRY) return UZCBR_CANCEL;
		break;
	default: // unknown...
		pgFillvram(RGB(255,0,0));
		mh_print(0,0,"Unzip fatal error.",0xFFFF);
		pgScreenFlipV();
        break;
    }
	return UZCBR_PASS;
}

int getZipDirAll(const char *path)
{
	ROM_INFO stRomInfo;
	
	zip_nfiles = 0;
	path_files[0] = 0;
	path_inzip[0] = 0;

	Unzip_setCallback(getZipDirCallback);
	int ret = Unzip_execExtract(path, (unsigned long)&stRomInfo);
	if (ret != UZEXR_OK)
		zip_nfiles=0;

	sort_files(zip_sortfiles, 0, zip_nfiles-1);
	
	return zip_nfiles;
}

void getZipDir(const char *path)
{
	char *p;
	int i, len;
	
	strcpy(files[0].name,"..");
	files[0].type = TYPE_DIR;
	sortfiles[0] = files;
	nfiles = 1;
	
	len = strlen(path);
	for(i=0; i<zip_nfiles; i++){
		if(strncmp(zip_sortfiles[i]->name,path,len)) continue;
		strcpy(files[nfiles].name,zip_sortfiles[i]->name + len);
		p = strchr(files[nfiles].name, '/');
		if(p){
			*(p+1) = 0;
			if(!strcmp(files[nfiles].name,files[nfiles-1].name)) continue;
			files[nfiles].type = TYPE_DIR;
		}else{
			files[nfiles].type = TYPE_FILE;
		}
		sortfiles[nfiles] = files + nfiles;
		nfiles++;
	}
	sort_files(sortfiles+1, 0, nfiles-2);
}

char filer_msg[256]={0};
int getFilePath(char *fullpath, u32 ext)
{
	int sel=0, top=0, rows=21, x, y, h, i, up=0, inzip=0, oldDirType;
	char path[MAX_PATH], oldDir[MAX_NAME], tmp[MAX_PATH], *p;
	
	path_inzip[0] = 0;
	
	strcpy(path, fullpath);
	p = strrchr(path, '/');
	if (p){
		p++;
		strcpy(tmp, p);
		*p = 0;
	}
	else{
		strcpy(path,"ms0:/");
	}

	getDir(path, ext);

	if (tmp[0]){
		for(i=0; i<nfiles; i++){
			if (!stricmp(sortfiles[i]->name,tmp)){
				sel = i;
				top = i-3;
				break;
			}
		}
	}
	
	for(;;){
		readpad();
		if(new_pad)
			filer_msg[0]=0;
		if(new_pad & CTRL_CIRCLE){
			if(sortfiles[sel]->type == TYPE_DIR){
				if(!strcmp(sortfiles[sel]->name,"..")){
					up=1;
				}else{
					if(inzip){
						strcat(path_inzip,sortfiles[sel]->name);
						getZipDir(path_inzip);
					}else{
						strcat(path,sortfiles[sel]->name);
						getDir(path, ext);
					}
					sel=0;
				}
			}else{
				if(!inzip){
					strcpy(tmp,path);
					strcat(tmp,sortfiles[sel]->name);
					if (getExtId(tmp)==EXT_ZIP){
						getZipDirAll(tmp);
						if(zip_nfiles!=1){
							strcat(path,sortfiles[sel]->name);
							getZipDir(path_inzip);
							sel=0;
							inzip=1;
						}else
							break;
					}else
						break;
				}else
					break;
			}
		}else if(new_pad & CTRL_CROSS){
			return 0;
		}else if(new_pad & CTRL_SELECT){
			if(!inzip && sortfiles[sel]->type == TYPE_FILE){
				strcpy(tmp,"\"");
				strcat(tmp,sortfiles[sel]->name);
				strcat(tmp,"\"\n\nRemove?");
				if(rin_MessageBox(tmp,1)){
					strcpy(tmp, path);
					strcat(tmp, sortfiles[sel]->name);
					if(sceIoRemove(tmp)>=0){
						strcpy(filer_msg,"Removed \"");
						strcat(filer_msg,sortfiles[sel]->name);
						strcat(filer_msg,"\"");
						getDir(path, ext);
					}
				}
			}
		}else if(new_pad & CTRL_TRIANGLE){
			up=1;
		}else if(new_pad & CTRL_UP){
			sel--;
		}else if(new_pad & CTRL_DOWN){
			sel++;
		}else if(new_pad & CTRL_LEFT){
			sel-=rows/2;
		}else if(new_pad & CTRL_RIGHT){
			sel+=rows/2;
		}
		
		if(up){
			oldDir[0]=0;
			oldDirType = TYPE_DIR;
			if(inzip){
				if(path_inzip[0]==0){
					oldDirType = TYPE_FILE;
					inzip=0;
				}else{
					path_inzip[strlen(path_inzip)-1]=0;
					p = strrchr(path_inzip,'/');
					if (p)
						p++;
					else
						p = path_inzip;
					sprintf(oldDir,"%s/", p);
					*p = 0;
					getZipDir(path_inzip);
					sel=0;
				}
			}
			if(strcmp(path,"ms0:/") && !inzip){
				if(oldDirType==TYPE_DIR)
					path[strlen(path)-1]=0;
				p=strrchr(path,'/')+1;
				strcpy(oldDir,p);
				if(oldDirType==TYPE_DIR)
					strcat(oldDir,"/");
				*p=0;
				getDir(path, ext);
				sel=0;
			}
			for(i=0; i<nfiles; i++) {
				if(oldDirType==sortfiles[i]->type
				&& !strcmp(oldDir, sortfiles[i]->name)) {
					sel=i;
					top=sel-3;
					break;
				}
			}
			up=0;
		}
		
		if(top > nfiles-rows)	top=nfiles-rows;
		if(top < 0)				top=0;
		if(sel >= nfiles)		sel=nfiles-1;
		if(sel < 0)				sel=0;
		if(sel >= top+rows)		top=sel-rows+1;
		if(sel < top)			top=sel;
		
		if(inzip){
			sprintf(tmp,"%s:/%s",strrchr(path,'/')+1,path_inzip);
			rin_frame(tmp,"○：OK  ×：Cancel  △：UP");
		}else
			rin_frame(filer_msg[0]?filer_msg:path,"○：OK  ×：Cancel  △：UP   SELECT：Remove");
		
		// スクロールバー
		if(nfiles > rows){
			h = 219;
			pgDrawFrame(445,25,446,248,setting.color[1]);
			pgFillBox(448, h*top/nfiles + 27,
				460, h*(top+rows)/nfiles + 27,setting.color[1]);
		}
		
		x=28; y=32;
		for(i=0; i<rows; i++){
			if(top+i >= nfiles) break;
			mh_print(x, y, sortfiles[top+i]->name, setting.color[top+i==sel?2:3]);
			y+=10;
		}
		
		pgScreenFlipV();
	}
	
	strcpy(fullpath, path);
	strcat(inzip?path_inzip:fullpath, sortfiles[sel]->name);
	return 1;
}
