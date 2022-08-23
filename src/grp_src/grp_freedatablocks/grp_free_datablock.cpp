/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2020
 */

#include "freedatablocks.h"
#include "bin_freedatablocks.h"
#include "grp_freedatablocks.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "core.h"
#include "devtools.h"
#include "daal.h"

namespace sofs21
{
    void grpFreeDataBlock(uint32_t bn)
    {
        soProbe(442, "%s(%u)\n", __FUNCTION__, bn);

        //Alexandre Paiva 89908
        
        SOSuperblock *sb =  soGetSuperblockPointer();

        if(0>bn || bn>sb->dbtotal) {//BN tem de estar no intervalo entre 0 e dbtotal(número total de datablocks)
            throw SOException(EINVAL,__FUNCTION__);//Criar exception EINVAL se o número total de datablocks não é válido 

        } else {//BN é válido
            if(sb->insertion_cache.idx >= REF_CACHE_SIZE) {//Se a insertion cache tiver cheia, chamar a deplete
               soDeplete();
            }
            sb->insertion_cache.ref[sb->insertion_cache.idx] = bn;//'The reference must be inserted at the first empty cell.'
            sb->insertion_cache.idx ++;//Atualizar o index da insertion cache
            sb->dbfree ++;//Incrementar o número de free data blocks
            soSaveSuperblock();
        }
    }
};

