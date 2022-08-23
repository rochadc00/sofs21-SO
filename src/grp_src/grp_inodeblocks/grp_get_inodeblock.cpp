/*
 *  \authur Artur Pereira - 2016-2021
 */

#include "inodeblocks.h"
#include "bin_inodeblocks.h"
#include "grp_inodeblocks.h"

#include "daal.h"
#include "core.h"
#include "devtools.h"

#include <errno.h>

namespace sofs21
{
    /* ********************************************************* */

#if false
    /* Considering bn is the number of a data block containing references to
     * data blocks, return the value of its idx position
     */
    static uint32_t grpGetIndirectInodeBlock(uint32_t bn, uint32_t idx);

    /* Considering bn is the number of a data block containing references
     * to data blocks containing references to data blocks (double indirection),
     * return the value of its idx position
     */
    static uint32_t grpGetDoubleIndirectInodeBlock(uint32_t bn, uint32_t idx);
#endif

    /* ********************************************************* */

    uint32_t grpGetInodeBlock(int ih, uint32_t ibn)
    {
        soProbe(301, "%s(%d, %u)\n", __FUNCTION__, ih, ibn);

        /* replace this comment and following line with your code */
        return binGetInodeBlock(ih, ibn);
    }

    /* ********************************************************* */

#if false
    static uint32_t grpGetIndirectInodeBlock(uint32_t bn, uint32_t idx)
    {
        soProbe(301, "%s(%d, %d)\n", __FUNCTION__, bn, idx);

        /* replace the following two lines with your code */
        throw SOException(ENOSYS, __FUNCTION__); 
        return 0;
    }
#endif

    /* ********************************************************* */

#if false
    static uint32_t grpGetDoubleIndirectInodeBlock(uint32_t bn, uint32_t idx)
    {
        soProbe(301, "%s(%d, %d)\n", __FUNCTION__, bn, idx);

        /* replace the following two lines with your code */
        throw SOException(ENOSYS, __FUNCTION__); 
        return 0;
    }
#endif
};

