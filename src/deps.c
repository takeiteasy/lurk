//
//  deps.c
//  wee
//
//  Created by George Watson on 24/08/2023.
//

#define SOKOL_IMPL
#include "sokol_gfx.h"
#if !defined(WEE_STATE)
#include "sokol_app.h"
#include "sokol_glue.h"
#endif
#include "sokol_args.h"
#include "sokol_time.h"
#define JIM_IMPLEMENTATION
#include "jim.h"
#define MJSON_IMPLEMENTATION
#include "mjson.h"
#define HASHMAP_IMPL
#include "hashmap.h"
#define QOI_IMPLEMENTATION
#include "qoi.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define EZ_IMPLEMENTATION
#include "ezimage.h"
#include "ezmath.h"
#include "ezrng.h"
#include "ezecs.h"
#include "ezfs.h"
