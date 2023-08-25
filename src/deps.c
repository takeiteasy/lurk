//
//  deps.c
//  wee
//
//  Created by George Watson on 24/08/2023.
//

#define SOKOL_IMPL
#define MJSON_IMPLEMENTATION
#define JIM_IMPLEMENTATION
#define HASHMAP_IMPL
#include "sokol_gfx.h"
#if !defined(WEE_STATE)
#include "sokol_app.h"
#include "sokol_glue.h"
#endif
#include "sokol_args.h"
#include "sokol_time.h"
#include "jim.h"
#include "mjson.h"
#include "hashmap.h"
