/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2020
 */

#include "freeinodes.h"
#include "bin_freeinodes.h"
#include "grp_freeinodes.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <iostream>

#include "core.h"
#include "devtools.h"
#include "daal.h"

namespace sofs21
{
    bool grpHideInode(uint16_t in)
    {
        soProbe(403, "%s(%u)\n", __FUNCTION__, in);
        
        auto* psb = soGetSuperblockPointer();
        if (in < 0 || in > psb->itotal)
            throw SOException(EINVAL, __FUNCTION__);
        if (psb->iqcount >= DELETED_QUEUE_SIZE)
            return false;
        auto ih = soOpenInode(in);
        auto* pin = soGetInodePointer(ih);
        pin->mode &= S_IFMT;
        soSaveInode(ih);
        soCloseInode(ih);
        psb->iqueue[(psb->iqcount++) % DELETED_QUEUE_SIZE] = in;
        soSaveSuperblock();
        return true;
    }
};

