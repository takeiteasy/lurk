//
//  test.c
//  weetest
//
//  Created by George Watson on 21/07/2023.
//

#include <stdio.h>
#include "wee.h"

static void init(void* _) {
    
}

static void frame(void* _, float delta) {
    
}

static void cleanup(void* _) {
    
}

int main(int argc, const char *argv[]) {
    weeSetInitCallback(init);
    weeSetFrameCallback(frame);
    weeSetCleanupCallback(cleanup);
    return weeInit(argc, argv);
}

