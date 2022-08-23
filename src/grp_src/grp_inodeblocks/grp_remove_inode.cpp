/*
 *  \authur Artur Pereira - 2016-2021
 */

#include "inodeblocks.h"
#include "bin_inodeblocks.h"
#include "grp_inodeblocks.h"
#include "freeinodes.h"

#include "daal.h"
#include "core.h"
#include "devtools.h"

#include <string.h>
#include <inttypes.h>

namespace sofs21
{
    void grpRemoveInode(uint16_t in)
    {
        soProbe(334, "%s(%d)\n", __FUNCTION__, in);
        if (!soHideInode(in)) {
            auto rin = soUnqueueHiddenInode();
            auto rih = soOpenInode(rin);
            auto* prin = soGetInodePointer(rih);
            soFreeInodeBlocks(rih, prin->d[0]);
            soSaveInode(rih);
            soCloseInode(rih);
            soFreeInode(rin);
        }
    }
};

