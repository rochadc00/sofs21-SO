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

#include <errno.h>

#include <iostream>

namespace sofs21
{

#if false
    static uint32_t grpAllocIndirectInodeBlock(int ih, uint32_t & i1, uint32_t idx);
    static uint32_t grpAllocDoubleIndirectInodeBlock(int ih, uint32_t & i2, uint32_t idx);
#endif

    /* ********************************************************* */

    uint32_t grpAllocInodeBlock(int ih, uint32_t ibn)
    {
        soProbe(302, "%s(%d, %u)\n", __FUNCTION__, ih, ibn);

        /* replace the following two lines with your code */
        return binAllocInodeBlock(ih, ibn);
    }

    /* ********************************************************* */

#if false
    /*
    */
    static uint32_t grpAllocIndirectInodeBlock(int ih, uint32_t & i1, uint32_t idx)
    {
        soProbe(302, "%s(%d, %u, %u)\n", __FUNCTION__, ih, i1, idx);

        /* replace the following two lines with your code */
        throw SOException(ENOSYS, __FUNCTION__); 
        return 0;
    }
#endif

    /* ********************************************************* */

#if false
    /*
    */
    static uint32_t grpAllocDoubleIndirectInodeBlock(int ih, uint32_t & i2, uint32_t idx)
    {
        soProbe(302, "%s(%d, %u, %u)\n", __FUNCTION__, ih, i2, idx);

        /* replace the following two lines with your code */
        throw SOException(ENOSYS, __FUNCTION__); 
        return 0;
    }
#endif
};

