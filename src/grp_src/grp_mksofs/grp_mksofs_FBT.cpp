#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <string.h>

namespace sofs21
{
    void grpFillInBitmapTable(uint32_t ntotal, uint16_t itsize, uint32_t dbtotal)
    {
        soProbe(605, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itsize, dbtotal);

        //Alexandre Paiva 89908
        
        //1 bloco = 1024*8 bits
        uint32_t block[RPB];
        if (dbtotal <= (BlockSize*8)) {//BASTA 1 BLOCO
            if (dbtotal>=32) {//dbtotal maior que 32
                uint32_t rest=dbtotal%32;
                uint32_t quoci=dbtotal/32;
                block[0]=0xFFFFFFFE;
                for(uint32_t l = 1;l<quoci;l++) {
                    block[l]=0xFFFFFFFF;
                }
                if (rest != 0) {//tem resto
                    block[quoci]=0xFFFFFFFF>>(32-rest);
                    quoci++;
                }
                for(uint32_t z=quoci;z<=RPB;z++) {
                    block[z]=0x00000000;
                }
                soWriteRawBlock(1+itsize,block);
            } else {//dbtotal menor que 32
                uint32_t block[RPB];
                block[0]=((0xFFFFFFFE>>(32-dbtotal))^0x1);
                for(uint32_t z=1;z<RPB;z++) {
                        block[z]=0x00000000;
                    }
                soWriteRawBlock(1+itsize,block);
            }
        } else {//PRECISA DE MAIS DO QUE UM BLOCO
            if((dbtotal%(BlockSize*8)) == 0) {//Número certos de Blocos

                block[0]=0xFFFFFFFE;//Primeiro Bloco
                for(uint32_t l = 1;l<RPB;l++) {
                    block[l]=0xFFFFFFFF;
                }
                soWriteRawBlock(1+itsize,block);

                for(uint32_t tr = 1;tr<=(dbtotal/(BlockSize*8));tr++) {//Restantes Blocos
                    for(uint32_t l = 0;l<RPB;l++) {
                        block[l]=0xFFFFFFFF;
                    }
                    soWriteRawBlock(1+itsize,block);
                }

            } else {//Número não certo de blocos
                block[0]=0xFFFFFFFE;//Primeiro Bloco
                for(uint32_t l = 1;l<RPB;l++) {
                    block[l]=0xFFFFFFFF;
                }
                soWriteRawBlock(1+itsize,block);

                for(uint32_t l = 0;l<RPB;l++) {//Restantes Blocos menos o últimos
                    block[l]=0xFFFFFFFF;
                }
                for(uint32_t ty = 1;ty<((dbtotal/(BlockSize*8))-1);ty++) {
                    soWriteRawBlock(1+itsize,block);
                }

                //Ultimo bloco
                uint32_t resto2 = (dbtotal%(BlockSize*8));
                uint32_t quoc3 = (resto2/32);
                uint32_t resto3 = (resto2%32);
                for(uint32_t kk = 0;kk<quoc3;kk++){
                    block[kk]=0xFFFFFFFF;
                }
                if (resto3 != 0) {
                    block[quoc3]=(0xFFFFFFFF>>(32-resto3));
                }
                for(uint32_t z=(quoc3+1);z<=RPB;z++) {
                    block[z]=0x00000000;
                }
                soWriteRawBlock(1+itsize,block);
            }
        }
    }
};

