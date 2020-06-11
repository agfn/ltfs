#include "bitmap.h"
#include "device.h"
#include "block.h"
#include <stdlib.h>
#include <assert.h>

static uint_t bv32 = 0xffffffff; 

/* */
long long __find_empty(uint_t *bmp, int length, uint_t mask)
{
    for (int i = 0; i < length; i++) {
        if (bmp[i] != bv32) {
            for (int j = 0; j < 32; j++) {
                if (!((bmp[i] >> j) & mask)) {
                    return i * 32 + j;
                }
            }
        }
    }

    return -1;
}

/* */
void __set_inuse(uint_t *bmp, int length, uint_t idx, uint_t mask)
{
    uint_t i = idx / 32, j = idx % 32;
    if (i < length) {
        bmp[i] |= mask << j;
    }
}

/* */
void __set_empty(uint_t *bmp, int length, uint_t idx, uint_t mask)
{
    uint_t i = idx / 32, j = idx % 32;
    if (i < length) {
        bmp[i] &= ~(mask << j);
    }
}
