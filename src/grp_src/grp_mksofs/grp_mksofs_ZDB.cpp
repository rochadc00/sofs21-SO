#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs21
{
    void grpZeroFreeDataBlocks(uint32_t ntotal, uint16_t itsize, uint32_t dbtotal)
    {
        soProbe(607, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itsize, dbtotal);

        /* replace this comment and following line with your code */
         // RPB representa o numero de referencias por bloco
        // nototal = nmr de blocos no device
        // itsizze = nmr de blocos no inode
        // dbtotal = nmr de data blocos
        int bloco[RPB]={0};
        int first= (itsize/IPB)+dbtotal+2;

        for(int i=first;i<ntotal-1;i++){
            soWriteRawBlock(i,bloco); // coloca a zero todos as posições de i que representam a referencia do bloco
        }
    }
};

