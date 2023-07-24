//
//  bool.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef bool_h
#define bool_h

#if defined(_MSC_VER) && _MSC_VER < 1800
#include <windef.h>
#define bool BOOL
#define true 1
#define false 0
#else
#if defined(__STDC__) && __STDC_VERSION__ < 199901L
typedef enum bool { false = 0, true = !false } bool;
#else
#include <stdbool.h>
#endif
#endif

#endif /* bool_h */
