/*
 * white_balance.c
 *
 *  Created on: 2016Äê5ÔÂ25ÈÕ
 *      Author: Thinkpad-T440p
 */
#include "white_balance.h"

//volatile double Kavg=0, Ravg=0, Gavg=0, Bavg=0;
//volatile uint32_t Kavg=0, Ravg=0, Gavg=0, Bavg=0;
//kval[0 r 1g 2b 4k]
int white_balance(unsigned int* input, unsigned int kval[4],unsigned int* output)
//void white_balance(uint32_t *input, uint32_t *output)
{
/*
#pragma AP INTERFACE ap_fifo port=input depth=2073600
#pragma AP INTERFACE ap_fifo port=output depth=2073600
*/

	uint32_t j,i;
	uint32_t Rsum=0, Gsum=0, Bsum=0;
	uint32_t red=0, green=0, blue=0;
	uint32_t Rnew, Gnew, Bnew;
	uint32_t thisPixel,index;

	for (i = 0; i < FRAME_HEIGHT; i++) {
		for (j = 0; j < FRAME_WIDTH; j++) {
#pragma AP PIPELINE II = 1
			index = (i * FRAME_WIDTH + j);
			thisPixel = input[index];

			red  = (uint32_t) getRedCMOS(thisPixel);
			Rsum += red;
			green = (uint32_t) getGreenCMOS(thisPixel);
			Gsum += green;
			blue = (uint32_t) getBlueCMOS(thisPixel);
			Bsum += blue;

/*			Rnew = red*Kavg/Ravg;
			if(Rnew > 255)	Rnew = 255;
			Gnew = green*Kavg/Gavg;
			if(Gnew > 255)	Gnew = 255;
			Bnew = blue*Kavg/Bavg;
			if(Bnew >255)	Bnew = 255;
			output[index]=(Rnew | (Gnew<<8) | (Bnew<<16));*/
			Rnew = red*kval[3]/kval[0];
			if(Rnew > 255)	Rnew = 255;
			Gnew = green*kval[3]/kval[1];
			if(Gnew > 255)	Gnew = 255;
			Bnew = blue*kval[3]/kval[2];
			if(Bnew >255)	Bnew = 255;
			output[index]=(Rnew | (Gnew<<8) | (Bnew<<16));
		}
	}
//#pragma HLS INLINE
/*
	Ravg = (double)Rsum / (double)(FRAME_HEIGHT*FRAME_WIDTH);
	Gavg = (double)Gsum / (double)(FRAME_HEIGHT*FRAME_WIDTH);
	Bavg = (double)Bsum / (double)(FRAME_HEIGHT*FRAME_WIDTH);
*/

/*	Ravg = (Rsum>>10)>>11;
	Gavg = (Gsum>>10)>>11;
	Bavg = (Bsum>>10)>>11;
	Kavg = (uint32_t)((Ravg+Gavg+Bavg)/3.0);*/
	kval[0] = (Rsum>>10)>>11;
	kval[1] = (Gsum>>10)>>11;
	kval[2] = (Bsum>>10)>>11;
	kval[3] = (uint32_t)((kval[0]+kval[1]+kval[2])/3.0);

	return 0;
}


