/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2020
 */

#include "freedatablocks.h"
#include "bin_freedatablocks.h"
#include "grp_freedatablocks.h"

#include "core.h"
#include "devtools.h"
#include "daal.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;

namespace sofs21
{
    /* only fill the current block to its end */
    void grpDeplete(void)
    {
        soProbe(444, "%s()\n", __FUNCTION__);

        /* replace this comment and following line with your code */
        binDeplete();
    }
};

