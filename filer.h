#ifndef FILER_H
#define FILER_H

extern SceIoDirent files[MAX_ENTRY];
extern int nfiles;
extern char path_files[];
extern char filer_msg[256];

long load_rom(const char *szRomPath);
int getFilePath(char *fullpath, u32 ext);

// —LŒø‚ÈŠg’£Žq
enum {
	EXT_GB = 1,
	EXT_GZ = 2,
	EXT_ZIP = 4,
	EXT_TCH = 8,
	EXT_UNKNOWN = 16,
};

#endif
