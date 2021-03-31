//
//  version.h
//  edge-lock
//
//  Created by Frank Boddeke on 18/06/2019.
//  Copyright © 2019 EdgeTechnologies. All rights reserved.
//

#ifndef version_h
#define version_h

// version history
// 0.0.1    20210322    created


// if major or minor update... also update makefile (too much work to incorporate these in the version)
#define FW_MAJOR   0
#define FW_MINOR   0

#include "build.h"

#define FW_CODE     ((FW_MAJOR<<12)+(FW_MINOR<<8)+FW_BUILD)

#endif /* version_h */
