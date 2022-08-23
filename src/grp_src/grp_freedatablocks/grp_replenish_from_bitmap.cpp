/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2021
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
    void grpReplenishFromBitmap(void)
    {
        soProbe(445, "%s()\n", __FUNCTION__);

        //Alexandre Paiva 89908
        
        SOSuperblock* sb = soGetSuperblockPointer();

        //Ver se a retrieval cache está vazia, se não estiver, não fazer nada (do nothing if the retrieval cache is not empty)
        if ((sb->retrieval_cache.idx)!=REF_CACHE_SIZE) {
            return;
        }
        
        if (sb->rbm_idx != NullBlockReference) {//Ver se o bitmap está vazio
            uint32_t cnt = 0;//Representa o número de referências passadas da bitmap table para a retrieval cache
            bool bitmapVazio = false;
            for(uint32_t z = 0; z < (sb->rbm_size);z++) {//Iterar todos os data blocks que fazem parte da bitmap table
                if (cnt == REF_CACHE_SIZE) {//Se já tivermos transferido as referências máximas (REF_CACHE_SIZE) para a bitmap table
                    break;
                }
                uint32_t data_blk = (sb->rbm_start)+z;//Ir buscar o data block com referências
                uint32_t refs[RPB];//Array de tamanho RPB (tamanho de um bloco)
                soReadDataBlock(data_blk, refs);//Ler o data block dado e guardar no array refs
                if ((sb->rbm_idx + REF_CACHE_SIZE) < RPB) {//Tudo num só bloco
                    for(uint32_t i = sb->rbm_idx; i <= REF_CACHE_SIZE; i++) {//Iterar todas as referências do data block até ao tamanho máximo da cache
                        uint32_t r = refs[i];
                        if (r != NullBlockReference) {//Ver se o bitmap não ficou vazio
                            sb->retrieval_cache.ref[cnt] = refs[i];//Copiar da Bitmap para a retrieval
                            refs[i] = NullBlockReference;//Limpar a referência no data block
                        } else {//Bitmap está vazio
                            bitmapVazio = true;
                            break;//Não vale a pena continuar a iterar já que o resto será NullBLockReference também
                        }
                        cnt++;
                    }
                    soWriteDataBlock(data_blk, refs);//Escrever no bloco as mudanças feitas
                    if (bitmapVazio == false) {//Se a bitmap table entretanto não ficou vazia, então atualizar o seu index
                        sb->rbm_idx += (REF_CACHE_SIZE+1);//Atualizar o index de onde começar a ir buscar as referências
                    } else {//Se a bitmap table ficou vazia, indicar o mesmo atualizando o index para NullBlockReference
                        sb->rbm_idx = NullBlockReference;//Atualizar o index a dizer que a bitmap está vazio
                    }
                } else {
                    uint32_t resto = abs(((sb->rbm_idx + REF_CACHE_SIZE) - RPB));//Representa o resto das referências a passar para a retrieval cache
                                                                                //da bitmap table de [0,REF_CACHE_SIZE[
                    for(uint32_t i = sb->rbm_idx; i < (REF_CACHE_SIZE-resto); i++) {//Iterar todas as referências do data block possíveis
                        uint32_t r = refs[i];
                        if (r != NullBlockReference) {//Ver se o bitmap não ficou vazio
                            sb->retrieval_cache.ref[cnt] = refs[i];//Copiar da Bitmap para a retrieval
                            refs[i] = NullBlockReference;//Limpar a referência no data block
                            cnt++;
                        } else {//Bitmap está vazio
                            bitmapVazio = true;
                            break;//Não vale a pena continuar a iterar já que o resto será NullBLockReference também
                        }
                    }
                    if (bitmapVazio == false) {//Se a bitmap table entretanto não ficou vazia, então continuar a passar referências
                        uint32_t refs2[RPB];//Array de tamanho RPB (tamanho de um bloco)
                        soReadDataBlock(data_blk+1, refs2);//Ler o data bloco dado +1, que indica o bloco onde continuam as referências da bitmap table
                        for(uint32_t i = 0; i < resto; i++) {//Iterar todas as referências do data block até ao tamanho máximo da cache
                            uint32_t r = refs[i];
                            if (r != NullBlockReference) {//Ver se o bitmap não ficou vazio
                                sb->retrieval_cache.ref[cnt] = refs2[i];//Copiar da Bitmap para a retrieval
                                refs2[i] = NullBlockReference;//Limpar a referência no data block
                                cnt++;
                            } else {//Bitmap está vazio
                                bitmapVazio = true;
                                break;//Não vale a pena continuar a iterar já que o resto será NullBLockReference também
                            }
                        }
                        soWriteDataBlock(data_blk+1, refs2);//Escrever no bloco as mudanças feitas
                        if (bitmapVazio == false) {//Se a bitmap table entretanto não ficou vazia, então atualizar o seu index
                            sb->rbm_idx = resto;//Atualizar o index de onde começar a ir buscar as referências
                        } else {//Se a bitmap table ficou vazia, indicar o mesmo atualizando o index para NullBlockReference
                            sb->rbm_idx = NullBlockReference;//Atualizar o index a dizer que a bitmap está vazio
                        }
                    } else {
                        sb->rbm_idx = NullBlockReference;//Atualizar o index a dizer que a bitmap está vazio
                    }
                }
            }
            sb->retrieval_cache.idx=cnt;//Atualizar a retrieval cache com o index correto
        } else {//Bitmap  está vazio
            return;//Return pois não precisamos de dar save ao superblock visto não ter efetuado qualquer mudança a nada do mesmo
        }
        soSaveSuperblock();
    }
};

