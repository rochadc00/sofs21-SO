/*
 *  \authur Artur Pereira - 2016-2021
 */

#include "inodeblocks.h"
#include "bin_inodeblocks.h"
#include "grp_inodeblocks.h"

#include "freedatablocks.h"
#include "daal.h"
#include "core.h"
#include "devtools.h"

#include <inttypes.h>
#include <errno.h>
#include <assert.h>

namespace sofs21
{
    /* free all blocks between positions ffbn and RPB - 1
     * existing in the block of references given by i1.
     * Return true if, after the operation, all references become NullBlockReference.
     * It assumes i1 is valid.
     */
    static bool grpFreeIndirectInodeBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn);

    /* free all blocks between positions ffbn and RPB**2 - 1
     * existing in the block of indirect references given by i2.
     * Return true if, after the operation, all references become NullBlockReference.
     * It assumes i2 is valid.
     */
    static bool grpFreeDoubleIndirectInodeBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn);

    /* ********************************************************* */

    void grpFreeInodeBlocks(int ih, uint32_t ffbn)
    {
        soProbe(303, "%s(%d, %u)\n", __FUNCTION__, ih, ffbn);

        //Alexandre Ferreira Martins Paiva 89908

        SOInode* ip = soGetInodePointer(ih);

        //Libertar blocos diretamente referenciados
        for(uint32_t i = 0; i < N_DIRECT; i++) {
            if(i>=ffbn) {
                if (ip->d[i] != NullBlockReference) {
                    soFreeDataBlock[ip->d[i]];
                    ip->d[i] = NullBlockReference;
                }
            }
        }
        
        //Libertar blocos indiretamente referenciados
        if(ffbn < N_DIRECT + RPB) {
            bool tr = grpFreeIndirectInodeBlocks(ip,ip->i1,ffbn);
            if (tr == true) {
                soFreeDataBlock(ip->i1);
                ip->i1=NullBlockReference;
            }
        }
        
        //Libertar blocos duplamente indiretamente referenciados
        if(ffbn < N_DIRECT + RPB + (RPB*RPB)) {
            bool tr = grpFreeDoubleIndirectInodeBlocks(ip,ip->i2,ffbn);
            if (tr == true) {
                soFreeDataBlock(ip->i2);
                ip->i2=NullBlockReference;
            }
        }

        soSaveInode(ih);
    }

    /* ********************************************************* */

    static bool grpFreeIndirectInodeBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i1, ffbn);

        uint32_t data_blk = i1;//Ir buscar o data block com refer??ncias
 
        if(data_blk!=NullBlockReference) {//Verificar se o bloco dado n??o est?? vazio
            //Libertar as refer??ncias do bloco
            bool limpou = false;//Vari??vel de controlo para saber se foi efetuadas mudan??as ao data block lido,
                                //para evitar chamar o write data block desnecessariamente
            uint32_t index;//Calcular o index do bloco sem o 'offset' do N_DIRECT
            if (ffbn < N_DIRECT) {
                index = 0;
            } else {
                index = ffbn-N_DIRECT;
            }
            uint32_t refs[RPB];//Array de tamanho RPB, visto ser um s?? bloco (tamanho de um bloco)
            soReadDataBlock(data_blk, refs);//Ler o data block dado com as refer??ncias diretas e guardar no array refs
            for(uint32_t i = index; i < RPB; i++) {//Iterar todas as refer??ncias do data block a partir do ffbn
                uint32_t r = refs[i];
                if (r != NullBlockReference) {//Se ainda n??o estiver limpa, limpar a mesma
                    soFreeDataBlock(r);//Libertar o data block referenciado
                    refs[i] = NullBlockReference;//Limpar a refer??ncia no data block
                    limpou = true;
                }
            }
            if (limpou == true) {
                soWriteDataBlock(data_blk, refs);//Escrever no bloco as mudan??as feitas
            }
            for(uint32_t i = 0; i < RPB;i++) {//Verificar se o bloco est?? totalmente vazio
                uint32_t r = refs[i];
                if (r != NullBlockReference) {
                    return false;//N??o est?? totalmente vazio
                }
            }
            return true;//Est?? totalmente vazio
        }
        return false;
    }
    /* ********************************************************* */

    static bool grpFreeDoubleIndirectInodeBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i2, ffbn);

        uint32_t blk = i2;//Ir buscar o data block com refer??ncias para blocks 

        if(blk!=NullBlockReference) {//Verificar se o bloco dado n??o est?? vazio
                uint32_t refs2[RPB];//Array de tamanho RPB, visto ser um s?? bloco (tamanho de um bloco)
                uint32_t nBlocos;
                uint32_t nBlocosRem;
                //Ver se o ffbn est?? antes do intervalo do i2 ou n??o
                if (ffbn > (N_DIRECT+RPB)) {
                    nBlocos =  (ffbn-(N_DIRECT+RPB))/RPB;//N??mero de blocos que ir??o ser limpos
                    nBlocosRem = (ffbn-(N_DIRECT+RPB))%RPB;//Bloco que n??o ir?? ser totalmente limpo
                } else {
                    nBlocos = 0;
                    nBlocosRem = 0;
                }
                bool limpou = false;
                soReadDataBlock(blk, refs2);
                if (nBlocosRem == 0) {//S?? ?? preciso limpar blocos no seu tamanho total
                    for(uint32_t i = nBlocos; i<RPB; i++) {
                        if(refs2[i] != NullBlockReference) {
                            //Libertar as refer??ncias do bloco
                            uint32_t refs4[RPB];//Refer??ncias para os data blocks guardados no data block
                            soReadDataBlock(refs2[i], refs4);
                            for(uint32_t k = 0; k < RPB; k++) {//Iterar todas as refer??ncias do data block
                                uint32_t r = refs4[k];
                                if (r != NullBlockReference) {
                                    soFreeDataBlock(r);//Libertar o data block referenciado
                                    refs4[k] = NullBlockReference;//Limpar a refer??ncia no data block
                                }
                            }
                            soWriteDataBlock(refs2[i], refs4);//N??o precisamos de uma vari??vel 'limpou' como no indirect pois sabemos que
                                                              //este ir?? ser modificado de certeza
                            refs2[i]=NullBlockReference;
                            limpou=true;
                        }
                    }
                } else {//Temos um bloco wque n??o ir?? ser limpo na totalidade
                    for(uint32_t i = nBlocos+1; i<RPB; i++) {//Fazer o mesmo que no for acima mas desta vez o primeiro bloco ser?? tratado '?? parte'
                        if(refs2[i] != NullBlockReference) {
                            //Libertar as refer??ncias do bloco
                            uint32_t refs4[RPB];//Refer??ncias para os data blocks guardados no data block
                            soReadDataBlock(refs2[i], refs4);
                            for(uint32_t k = 0; k < RPB; k++) {//Iterar todas as refer??ncias do data block
                                uint32_t r = refs4[k];
                                if (r != NullBlockReference) {
                                    soFreeDataBlock(r);//Libertar o data block referenciado
                                    refs4[k] = NullBlockReference;//Limpar a refer??ncia no data block
                                }
                            }
                            soWriteDataBlock(refs2[i], refs4);//N??o precisamos de uma vari??vel 'limpou' como no indirect pois sabemos que
                                                              //este ir?? ser modificado de certeza
                            refs2[i]=NullBlockReference;
                            limpou=true;
                        }
                    }
                    if(refs2[nBlocos] != NullBlockReference) {//'Tratar' do bloco que foi posto '?? parte' do ciclo acima
                            //Libertar as refer??ncias do bloco
                            uint32_t refs7[RPB];//Refer??ncias para os data blocks guardados no data block
                            soReadDataBlock(refs2[nBlocos], refs7);
                            for(uint32_t i = nBlocosRem; i < RPB; i++) {//Iterar todas as refer??ncias do data block, a partir do index dado
                                uint32_t r = refs7[i];
                                if (r != NullBlockReference) {
                                    soFreeDataBlock(r);//Libertar o data block referenciado
                                    refs7[i] = NullBlockReference;//Limpar a refer??ncia no data block
                                }
                            }
                            soWriteDataBlock(refs2[nBlocos], refs7);
                            //Verificar agora se ap??s a limpeza o bloco ficou totalmente vazio
                            bool naoVazio = false;
                            for(uint32_t z = 0; z < RPB; z++) {
                                uint32_t r = refs7[z];
                                if (r != NullBlockReference) {
                                    naoVazio = true;
                                }
                            }
                            if (naoVazio == false) {//Se o bloco ficou totalmente vazio, ent??o libertar o bloco em si
                                refs2[nBlocos]=NullBlockReference;
                            }
                            limpou=true;
                        }
                }
                if (limpou == true) {
                        soWriteDataBlock(blk, refs2);//Atualizar o bloco de refer??ncias
                    }
                //Verificar se o bloco em si ficou totalmente vazio
                for(uint32_t i = 0; i<RPB; i++) {
                    uint32_t r = refs2[i];
                        if (r != NullBlockReference) {
                            return false;
                        }
                }
                return true;
        }
        return false;
    }
    /* ********************************************************* */
};

