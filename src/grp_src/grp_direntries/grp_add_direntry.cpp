#include "direntries.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"
#include "inodeblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs21
{
    void grpAddDirentry(int pih, const char *name, uint16_t cin)
    {
        soProbe(202, "%s(%d, %s, %u)\n", __FUNCTION__, pih, name, cin);

        /* replace this comment and following line with your code */
        // pih is a valid inode handler of a directory where the user has write access
        // name is a valid base name(it doesn't contain '/')
        // cin is a valid inode number
        // DPB number os direntries per block 

        // strncpy copy no more than N bytes chars of SRC to DEs
        // strncmp compare N characters of S1 and S2 

       SOInode* p = soGetInodePointer(pih);
       SODirectorySlot d[DPB]; //array of directory slots

       bool isEmpty = false;
       uint32_t bloco;
       uint16_t entrada;
       // uint32_t bloco = (p -> BlockSize/sizeof(SODirectorySlot))/RPB
       
       uint32_t totalBlocks = p -> size/BlockSize;
       uint32_t i=0;

        for (; i < totalBlocks; i++) {

            soReadInodeBlock(pih,i,d);

            uint32_t j = 0;
            for (; j < DPB; j++ ) {
                // exception EEXIST is thrown when the entry with the same name already exists
                if (strncmp(name, d[j].nameBuffer,DIRECTORY_SLOT) == 0) {
                    throw SOException(EEXIST,__FUNCTION__);
                }
                //empty slot
                if (!isEmpty && d[j].nameBuffer[0] == '\0') {
                    bloco = i;
                    entrada = j;
                    isEmpty = true;
                }

            //     char bbbb[31];
            //     snprintf(bbbb,31,"%s",d[j].nameBuffer);
            //     printf("%s\n\r",bbbb);


            //    printf("%u\n\r",(unsigned int)d[j].in);

            }
        }

        if(isEmpty)
        {
            soReadInodeBlock(pih,bloco,d);
            strncpy(d[entrada].nameBuffer,name,DIRECTORY_SLOT);
            d[entrada].in= cin;
            soWriteInodeBlock(pih,bloco,d);
        } else {
            bloco = soAllocInodeBlock(pih, totalBlocks);
            strncpy(d[0].nameBuffer, name,DIRECTORY_SLOT);
            d[0].in = cin;
            uint32_t j = 1;
            for(; j < DPB; j++){
                d[j].in = NullInodeReference;
                memset(d[j].nameBuffer,'\0',DIRECTORY_SLOT);
          }
            soWriteDataBlock(bloco,d);
            p->size+= BlockSize;
        }
        soSaveInode(pih); 

       // binAddDirentry(pih, name, cin);
    }
};

