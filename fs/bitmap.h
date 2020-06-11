#ifndef __bit_map_h__
#define __bit_map_h__

#include "types.h"
#include "fs.h"

/* return index */
long long __find_empty(uint_t *bmp, int length, uint_t mask);

/* */
void __set_inuse(uint_t *bmp, int length, uint_t idx, uint_t mask);

/* */
void __set_empty(uint_t *bmp, int length, uint_t idx, uint_t mask);


#endif 