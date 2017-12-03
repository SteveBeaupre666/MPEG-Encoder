#include "ColorConversion.h"

void move_420_block(int yTL, int yTR, int yBL, int yBR, int u, int v, int pitch, int bpp, BYTE *buf, bool bgr)
{
	const int rvScale = 91881;
	const int guScale = -22553;
	const int gvScale = -46801;
	const int buScale = 116129;
	const int yScale  = 65536;

	int r, g, b;
	g = guScale * u + gvScale * v;
	if(!bgr){
		r = buScale * u;
		b = rvScale * v;
	} else {
		r = rvScale * v;
		b = buScale * u;
	}

	yTL *= yScale; yTR *= yScale;
	yBL *= yScale; yBR *= yScale;

	// Write out top two pixels
	int i = 0;
	buf[i++] = LIMIT(b+yTL); 
	buf[i++] = LIMIT(g+yTL);
	buf[i++] = LIMIT(r+yTL);
	if(bpp == 4)
		buf[i++] = 0;

	buf[i++] = LIMIT(b+yTR); 
	buf[i++] = LIMIT(g+yTR);
	buf[i++] = LIMIT(r+yTR);
	if(bpp == 4)
		buf[i++] = 0;

	// Skip down to next line to write out bottom two pixels
	buf += pitch * bpp;

	// Write bottom two pixels
	i = 0;
	buf[i++] = LIMIT(b+yBL); 
	buf[i++] = LIMIT(g+yBL);
	buf[i++] = LIMIT(r+yBL);
	if(bpp == 4)
		buf[i++] = 0;

	buf[i++] = LIMIT(b+yBR); 
	buf[i++] = LIMIT(g+yBR);
	buf[i++] = LIMIT(r+yBR);
	if(bpp == 4)
		buf[i++] = 0;
}

void yuv420p_to_rgb(BYTE *pY, BYTE *pU, BYTE *pV, BYTE *pOut, int width, int height, int pitch, int bpp)
{
	BYTE *buf = pOut;
	int pad = pitch - width;

	for(int y = 0; y <= height - 2; y += 2){
		for(int x = 0; x <= width - 2; x += 2){

			int y00 = *(pY);
			int y01 = *(pY + 1);
			int y10 = *(pY + width);
			int y11 = *(pY + width + 1);

			int u = (*pU++) - 128;
			int v = (*pV++) - 128;

			move_420_block(y00, y01, y10, y11, u, v, pitch, bpp, buf);

			pY  += 2;
			buf += 2 * bpp;
		}

		pY  += width;
		buf += (pad + pitch) * bpp;
	}
}

void rgb24_to_yuv420p(BYTE *lum, BYTE *cb, BYTE *cr, BYTE *src, int width, int height)
{
    int wrap, wrap3, x, y;
    int r, g, b, r1, g1, b1;
    BYTE *p;

    wrap = width;
    wrap3 = width * 3;
    p = src;
    for(y=0;y<height;y+=2) {
        for(x=0;x<width;x+=2) {
            r = p[0];
            g = p[1];
            b = p[2];
            r1 = r;
            g1 = g;
            b1 = b;
            lum[0] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
            r = p[3];
            g = p[4];
            b = p[5];
            r1 += r;
            g1 += g;
            b1 += b;
            lum[1] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
            p += wrap3;
            lum += wrap;

            r = p[0];
            g = p[1];
            b = p[2];
            r1 += r;
            g1 += g;
            b1 += b;
            lum[0] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
            r = p[3];
            g = p[4];
            b = p[5];
            r1 += r;
            g1 += g;
            b1 += b;
            lum[1] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;

            cb[0] = ((- FIX(0.16874) * r1 - FIX(0.33126) * g1 +
                      FIX(0.50000) * b1 + 4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;
            cr[0] = ((FIX(0.50000) * r1 - FIX(0.41869) * g1 -
                     FIX(0.08131) * b1 + 4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            cb++;
            cr++;
            p += -wrap3 + 2 * 3;
            lum += -wrap + 2;
        }
        p += wrap3;
        lum += wrap;
    }
}

