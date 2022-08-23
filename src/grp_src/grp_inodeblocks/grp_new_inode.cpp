/*
 *  \authur Artur Pereira - 2016-2021
 */

#include "inodeblocks.h"
#include "bin_inodeblocks.h"
#include "grp_inodeblocks.h"
#include "freeinodes.h"
#include "time.h"

#include "daal.h"
#include "core.h"
#include "devtools.h"

#include <string.h>
#include <inttypes.h>
#include <unistd.h>

namespace sofs21
{
    uint16_t grpNewInode(uint16_t type, uint16_t perm)
    {
        soProbe(333, "%s(%04x, %9o)\n", __FUNCTION__, type, perm);

        /* replace this comment and following line with your code */
        //IPB number of inodes por bloco
        //MAX_INO/* replace this comment and following line with your code */

        //IPB number of inodes por bloco
        //MAX_INODES maximum number of inodes in a sofs21 disk

        //ENOSPC		28	/* No space left on device */
        //EINVAL		22	/* Invalid argument */


        if(!(type == S_IFREG || type == S_IFDIR || type == S_IFLNK)){
            throw SOException(EINVAL,__FUNCTION__);
        }

        if (perm < 0 || perm > 777) {
            throw SOException(EINVAL,__FUNCTION__);
        }

        SOSuperblock* sb = soGetSuperblockPointer();
        // uint16_t n = soAllocInode();
        // auto* p = soGetInodePointer(n);

        // p -> mode = type | perm;
        // p->atime = time(NULL);
        // p->ctime = time(NULL);
        // p->mtime = time(NULL);

        // p->owner = getuid();
        // p->group = getgid();

        // soSaveInode(n);

        if(sb->ifree == 0){
            throw SOException(ENOSPC, __FUNCTION__);
        }

        uint16_t inodeID = sb-> iidx;

        int inodeHandler = soOpenInode(inodeID);
        soAllocInode();
        auto* p= soGetInodePointer(inodeHandler);

        p->mode = type | perm;
        p->atime = time(NULL);
        p->ctime = time(NULL);
        p->mtime = time(NULL);

        p->owner = getuid();
        p->group = getgid();

        if(sb -> ifree == 0 && sb -> iqcount != 0){
            uint16_t del = soUnqueueHiddenInode();
            soFreeInodeBlocks(inodeHandler,del);
        }

        soSaveInode(inodeHandler);
        soCloseInode(inodeHandler);
        soSaveSuperblock();

        return inodeID;
    }
};

