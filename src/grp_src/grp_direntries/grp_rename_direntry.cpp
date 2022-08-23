#include "direntries.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"
#include "inodeblocks.h"
#include "bin_direntries.h"

#include <string.h>
#include <errno.h>
#include <sys/stat.h>

namespace sofs21
{
    void grpRenameDirentry(int pih, const char *name, const char *newName)
    {
        soProbe(204, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name, newName);

        /* replace this comment and following line with your code */
        //binRenameDirentry(pih, name, newName);

       SOInode* p = soGetInodePointer(pih);
       SODirectorySlot d[DPB]; //array of directory slots

       uint32_t bloco;
       uint16_t entrada;
       // uint32_t bloco = (pin -> BlockSize/sizeof(SODirectorySlot))/RPB
       
       uint32_t totalBlocks = p -> size/BlockSize;
       uint32_t i=0;

        for (; i < totalBlocks; i++) {

            soReadInodeBlock(pih,i,d);

            uint32_t j = 0;
            for (; j < DPB; j++ ) {
                if (strncmp(d[j].nameBuffer,name,DIRECTORY_SLOT)==0) {
                    bloco = i;
                    entrada = j;
                }
            }
        }

        SODirectorySlot novo [DPB];

        strncpy(novo[entrada].nameBuffer,newName,DIRECTORY_SLOT);
        soWriteInodeBlock(pih,bloco,novo);
    }
};
