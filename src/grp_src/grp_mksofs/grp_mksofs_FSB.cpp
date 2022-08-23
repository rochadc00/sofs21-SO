#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "devtools.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>
#include <algorithm>

// Number of superblocks per block
#define SPB (BlockSize/sizeof(SOSuperblock))

namespace mem
{
    template<typename T, std::size_t N>
    constexpr auto size_of(const T (&arr)[N]) -> std::size_t
    {
        return sizeof(arr)/sizeof(T);
    }
}

namespace sofs21
{
    void grpFillInSuperblock(const char *name, uint32_t ntotal, uint16_t itsize, uint32_t dbtotal)
    {
        soProbe(602, "%s(%s, %u, %u, %u)\n", __FUNCTION__, name, ntotal, itsize, dbtotal);

        /*
            WinAPI-Like struct initialization
        */
        SOSuperblock sb;
        /* Header */
        sb.magic = ~0; // file system identification number
        sb.version = VERSION_NUMBER; // version number
        sb.mntstat = 0x00; // mount status and mount count
        if (strlen(name) <= PARTITION_NAME_LEN) // volume name
            strcpy(sb.name, name);
        sb.ntotal = ntotal; // total number of blocks in the device
        /* INode's Metadata */
        sb.itotal = IPB*itsize; // total number of inodes
        sb.ifree = sb.itotal-1; // number of free inodes
        sb.iidx = 1; // bit index where search for a free inode should start from
        // bitmap representing inode allocation states
        std::fill_n(sb.ibitmap, mem::size_of(sb.ibitmap), ~(-1));
        for (uint16_t i = 0; i < sb.itotal; i++) {
            auto* word = &sb.ibitmap[i/(sizeof(uint32_t)*8)];
            *word = *word | 1 << (i%(sizeof(uint32_t)*8));
        }
        sb.ibitmap[0] = sb.ibitmap[0] & ~1;
        // queue of deleted inodes, managed in a circular FIFO
        std::fill_n(sb.iqueue, mem::size_of(sb.iqueue), NullInodeReference);
        sb.iqhead = 0; // index of oldest reference in iqueue
        sb.iqcount = 0; // number of elements in iqueue
        /* Data block's Metadata */
        sb.dbtotal = dbtotal; // total number of data blocks
        sb.dbp_start = sb.ntotal-sb.dbtotal; // physical number of the block where the data block pool starts
        sb.dbfree = sb.dbtotal-1; // number of free data blocks
        /* Reference bitmap's metadata */
        sb.rbm_start = SPB+itsize; // physical number of the block where the reference bitmap starts
        sb.rbm_size = 1; // TODO! number of blocks the reference bitmap comprises
        sb.rbm_idx = 0; // index of first 32-word to be used to retrieve references from bitmap
        /* Retriveral Cache Data */
        sb.retrieval_cache.idx = mem::size_of(sb.retrieval_cache.ref); // index of first free/occupied cell
        std::fill_n(sb.retrieval_cache.ref, mem::size_of(sb.retrieval_cache.ref), NullBlockReference);
        /* Insertion Cache Data */
        sb.insertion_cache.idx = 0; // index of first free/occupied cell
        std::fill_n(sb.insertion_cache.ref, mem::size_of(sb.retrieval_cache.ref), NullBlockReference);

        soWriteRawBlock(0, static_cast<void*>(&sb));
    }
};
