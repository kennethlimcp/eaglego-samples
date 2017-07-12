/*
 * zoom.c
 *
 *  Created on: 2016Äê7ÔÂ8ÈÕ
 *      Author: Thinkpad-T440p
 */
#include "zoom.h"
//Zoom
void zoom(unsigned int input[1024], float tgt_img[3][32][32])
{
	ARGB32* rgbIMG;
	float fw = Src_W/Dst_W;
	float fh = Src_H/Dst_H;
	int zj,zk;
	int src_h, src_w;
	for(zj=0; zj<Dst_W; zj++){
		src_h = (int)(zj*fh);
		for(zk=0; zk<Dst_H; zk++){
#pragma AP PIPELINE II = 1
			src_w = (int)(zk*fw);
			rgbIMG = (ARGB32*)(input+src_h*FRAME_WIDTH+src_w+420);	//420 for 1080*1080
			tgt_img[0][zj][zk] = (float)((*rgbIMG).rgbR);	//rgbB
			tgt_img[1][zj][zk] = (float)((*rgbIMG).rgbG);	//rgbG
			tgt_img[2][zj][zk] = (float)((*rgbIMG).rgbB);	//rgbR
		}
	}

}
