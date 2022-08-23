/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2020
 */

#include "freedatablocks.h"
#include "bin_freedatablocks.h"
#include "grp_freedatablocks.h"

#include <string.h>
#include <errno.h>
#include <iostream>

#include "core.h"
#include "devtools.h"
#include "daal.h"

namespace sofs21
{
    void grpReplenishFromCache(void)
    {
        soProbe(443, "%s()\n", __FUNCTION__);

        //Alexandre Paiva 89908
        
        SOSuperblock* sb = soGetSuperblockPointer();

        //Ver se a retrieval cache está vazia, se não estiver, não fazer nada (do nothing if the retrieval cache is not empty)
        if ((sb->retrieval_cache.idx)!=REF_CACHE_SIZE) {
            return;
        }
        
        if ((sb->insertion_cache.idx) > 0) {//Ver se a insertion cache está vazia, se estiver não fazer nada

            uint32_t quantidadeInsertion = sb->insertion_cache.idx;//Descobrir a quantidade da insertion cache que é para transferir

            for(uint32_t i = 0;i<quantidadeInsertion;i++) {
                sb->retrieval_cache.ref[i] = sb->insertion_cache.ref[i];//Copiar da Insertion para a retrieval
                sb->insertion_cache.ref[i] = NullBlockReference;//Atualizar a referência mudada da Insertion para NullBlockReference
            }
            sb->insertion_cache.idx=0;//Atualizar o index da insertion cache
            sb->retrieval_cache.idx=sb->insertion_cache.idx;//Atualizar a retrieval cache com o index correto

        } else {//Insertion cache está vazia
            return;
        }
        soSaveSuperblock();
    }
};

