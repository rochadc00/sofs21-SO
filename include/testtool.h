/**
 * 
 *
 * \defgroup testtool testtool
 * \ingroup devtools
 * 
 * \brief The intermediate functions (\ref ilayer "ILayer") test tool
 * 
 * \details
 *   A menu driven test tool, allowing for testing the intermediate functions.
 */

/*
 *  \author Artur Pereira 2007-2009, 2016-2020
 *  \author Miguel Oliveira e Silva 2009, 2017
 *  \author António Rui Borges - 2010--2015
 *
 */


#ifndef __SOFS21_TESTTOOLS__
#define __SOFS21_TESTTOOLS__

#include <stdio.h>

extern const char *devname;

extern char *progDir;

extern int quiet;

/* msgs */
void promptMsg(const char *fmt, ...);
void resultMsg(const char *fmt, ...);
void errorMsg(const char *fmt, ...);
void errnoMsg(int en, const char *fmt, ...);
void fPurge(FILE * fin);

/* core */
void notImplemented();
void setProbeIDs();
void addProbeIDs();
void removeProbeIDs();
void printProbeIDs();
void createDisk();
void formatDisk();
void showBlock();
void setBinIDs();
void addBinIDs();
void removeBinIDs();
void printBinIDs();

/* inodeattrs */
void setInodeSize();
void setInodeAccess();
void changeInodeOwnership();
void checkInodeAccess();
void decInodeLnkcnt();
void incInodeLnkcnt();

/* freeinodes stuff */
void allocInode();
void freeInode();
void ttHideInode();
void ttUnqueueHiddenInode();

/* freedatablocks stuff */
void allocDataBlock();
void freeDataBlock();
void replenishFromCache();
void replenishFromBitmap();
void depleteInsertionCache();

/* inodeblocks */
void getInodeBlock();
void allocInodeBlock();
void freeInodeBlocks();
void readInodeBlock();
void writeInodeBlock();
void ttNewInode();
void ttRemoveInode();

/* direntries */
void checkDirectoryEmptiness();
void getDirentry();
void addDirentry();
void renameDirentry();
void deleteDirentry();
void traversePath();

#endif /*  __SOFS21_TESTTOOLS__  */
