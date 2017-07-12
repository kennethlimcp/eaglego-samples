/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 * main.c: simple hdmi out application
 * by RD
 * 2015.08.04
 *
 */
#include <stdlib.h>
#include "sds_lib.h"
#include <stdio.h>
#include "imx222.h"
#include "xparameters.h"
#include "xil_cache.h"
#include "zoom.h"
#include "xspips.h"
#include "xgpiops.h"
#include "v3_rgbout.h"
#include "v3_rgbin.h"
#include "identi_cal.h"
#include "conv1.h"
#include "conv2.h"
#include "im.h"
#include "image_label.h"
//#include "white_balance.h"
#include "copy_buffer.h"



#define Src_H 1080
#define Src_W 1080
#define Dst_H 32
#define Dst_W 32

/* Definitions for peripheral PLDMA_RGBIN_0 */
#define XPAR_PLDMA_RGBIN_0_BASEADDR 0x40010000
#define XPAR_PLDMA_RGBIN_0_HIGHADDR 0x4001FFFF

/* Definitions for peripheral PLDMA_RGBOUT_0 */
#define XPAR_PLDMA_RGBOUT_0_BASEADDR 0x40020000
#define XPAR_PLDMA_RGBOUT_0_HIGHADDR 0x4000FFFF

/* Definitions for peripheral PS7_SPI_0 */
#define XPAR_PS7_SPI_0_DEVICE_ID 0

unsigned int mrd_addr = XPAR_PLDMA_RGBOUT_0_BASEADDR;
unsigned int mwr_addr = XPAR_PLDMA_RGBIN_0_BASEADDR;

int main() {

	unsigned int Status;
	Xil_DCacheDisable();
	// use ps gpio emio to
	XGpioPs gpiops;
	XGpioPs_Config *ConfigPtrPS;
	ConfigPtrPS = XGpioPs_LookupConfig(0);
	XGpioPs_CfgInitialize(&gpiops, ConfigPtrPS, ConfigPtrPS->BaseAddr);
	XGpioPs_SetDirectionPin(&gpiops, 54, 1);
	XGpioPs_SetOutputEnablePin(&gpiops, 54, 1);
	XGpioPs_SetDirectionPin(&gpiops, 55, 1);
	XGpioPs_SetOutputEnablePin(&gpiops, 55, 1);
	XGpioPs_SetDirectionPin(&gpiops, 56, 1);
	XGpioPs_SetOutputEnablePin(&gpiops, 56, 1);
	XGpioPs_SetDirectionPin(&gpiops, 57, 1);
	XGpioPs_SetOutputEnablePin(&gpiops, 57, 1);

	XGpioPs_WritePin(&gpiops, 55, 0x1); 	//hdmi outout enable
	XGpioPs_WritePin(&gpiops, 56, 0x1); 	//led on
	XGpioPs_WritePin(&gpiops, 57, 0x1);		//coms reset_n

	/*init spi*/
	XSpiPs spips;
	XSpiPs_Config* spi_config;
	spi_config = XSpiPs_LookupConfig(0);
	Status = XSpiPs_CfgInitialize(&spips, spi_config, spi_config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XSpiPs_Reset(&spips);
	XSpiPs_SetOptions(&spips,
			XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION);
	XSpiPs_SetClkPrescaler(&spips, XSPIPS_CLK_PRESCALE_32);
	XSpiPs_SetSlaveSelect(&spips, 0);
	initialIMX122(&spips);

	//to reset bayer2rgb after cmos config done
	XGpioPs_WritePin(&gpiops, 54, 0x1);		// cmos config done
	XGpioPs_WritePin(&gpiops, 54, 0x0);

	// initial pldma_rgbin
	char cmos_writing_index;
	unsigned int* pVideoBuffer;
	float tgt_img[3][32][32];
	//uint32_t kval[4]= {0};
	//unsigned int* kval = (unsigned int*) sds_alloc(sizeof(uint32_t) * 4);	//
	//sds_alloc_non_cacheable
	unsigned int* pDstBuffer = (unsigned int*) sds_alloc_non_cacheable(
			2048 * 2048 * sizeof(uint32_t) * 3);	//cmos
	unsigned int* DisplayBuf = (unsigned int*) sds_alloc_non_cacheable(
			FRAME_HEIGHT * FRAME_WIDTH * sizeof(uint32_t));	//hdmi
	//unsigned int* pDstBuffer0 = (unsigned int*) sds_alloc_non_cacheable(
			//FRAME_HEIGHT * FRAME_WIDTH * sizeof(uint32_t));	//
	video_in_init(mwr_addr, (unsigned int*) pDstBuffer, FRAME_WIDTH, FRAME_HEIGHT);
	// initial pldma_rgbout
	video_out_init(mrd_addr);

	while (1) {
		cmos_writing_index = get_video_in_index(mwr_addr);
		if (cmos_writing_index == 0)
			cmos_writing_index = 2;
		else
			cmos_writing_index = cmos_writing_index - 1;

		set_video_in_index(mwr_addr, cmos_writing_index);
		pVideoBuffer = (unsigned int*) (pDstBuffer + (cmos_writing_index << 22));
		float rst1[conv1_K][conv1_I][conv1_J] = { { { 0, 0, 0 } } };//need to be reset 0 every time?
		float rst2[conv2_K][conv2_I][conv2_J] = { { { 0, 0, 0 } } };//need to be reset 0 every time?
		float rst_ip1[cal_O] = { 0 };
		// white balance
		//white_balance(pVideoBuffer, pDstBuffer0);
		//white_balance(pVideoBuffer,kval,pDstBuffer0);
		//Zoom
		zoom(pVideoBuffer, tgt_img);
		//zoom(pDstBuffer0, tgt_img);
		//Classification
		conv1(tgt_img, rst1);
		conv2(rst1, rst2);
		//float* rst2_p = (float*) rst2;
		//fc
		identi_cal((float*)rst2, rst_ip1);
		//classification
		int i;
		int item_class = 0;
		float Pmax = -10;
		for (i = 0; i < cal_O; i++) {
			if (rst_ip1[i] > Pmax) {
				Pmax = rst_ip1[i];
				item_class = i;
			}
		}
		//printf("%s,%f\n", label[item_class], Pmax);
		// the label image is 150*200 pixel
		for (i = 0; i < 200; i++) {
			memcpy(pVideoBuffer + 1920 * i,
					label_array[item_class] + 150 * i * 4, 150 * 4);// pDstBuffer0 pVideoBuffer
		}
		//memcpy(DisplayBuf, pDstBuffer0, 1920 * 1080 * 4);//pDstBuffer0 pVideoBuffer
		copy_buffer(pVideoBuffer,DisplayBuf);
		video_out_start(mrd_addr, DisplayBuf);//直接显示会有撕裂,画面不如黑屏;
	}

	video_out_reset(mrd_addr);
	video_in_reset(mwr_addr);

	return 0;
}

