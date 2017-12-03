#pragma once

#include <Windows.h>
#include <stdio.h>

#define SCALEBITS 8
#define ONE_HALF  (1 << (SCALEBITS - 1))
#define FIX(x)    ((int) ((x) * (1L<<SCALEBITS) + 0.5))
#define LIMIT(x) ((x)>0xffffff?0xff: ((x)<=0xffff?0:((x)>>16)))

void move_420_block(int yTL, int yTR, int yBL, int yBR, int u, int v, int pitch, int bpp, BYTE *buf, bool bgr = false);
void yuv420p_to_rgb(BYTE *pY, BYTE *pU, BYTE *pV, BYTE *pOut, int width, int height, int pitch, int bpp);
void rgb24_to_yuv420p(BYTE *lum, BYTE *cb, BYTE *cr, BYTE *src, int width, int height);

