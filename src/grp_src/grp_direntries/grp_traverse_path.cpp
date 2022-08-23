#include "direntries.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"
#include "inodeblocks.h"
#include "direntries.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

namespace sofs21
{
    uint16_t traversePath(char* path);

    uint16_t grpTraversePath(char *path)
    {
        soProbe(221, "%s(%s)\n", __FUNCTION__, path);
        uint16_t bdin = traversePath(dirname(strdupa(path)));
        return soGetDirentry(bdin, basename(strdupa(path)));
    }

    uint16_t traversePath(char* path)
    {
        if (strcmp(path, "/") == 0)
            return 0;
        uint16_t ih = traversePath(dirname(strdupa(path)));
        if (soCheckInodeAccess(ih, X_OK) || false /* check if is dir */)
            throw SOException(ENOTDIR, __FUNCTION__);
        return ih;
    }
};
